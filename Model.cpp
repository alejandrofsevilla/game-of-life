#include "Model.hpp"

#include <chrono>
#include <map>
#include <random>
#include <thread>

namespace {
constexpr auto f_underpopulationThreshold{2};
constexpr auto f_overpopulationThreshold{3};
constexpr auto f_reproductionValue{3};
constexpr auto f_modelUpdatePeriod{std::chrono::milliseconds{1000}};
constexpr std::size_t f_defaultSpeed{5};
constexpr std::size_t f_maxSpeed{10};
constexpr std::size_t f_minSpeed{1};

inline std::size_t generateRandomValue(std::size_t min, std::size_t max) {
  std::random_device rd;
  std::mt19937 gen{rd()};
  std::uniform_int_distribution<std::size_t> distr{min, max};
  return distr(gen);
}
}  // namespace

Model::Model(std::size_t width, std::size_t height)
    : m_width{width},
      m_height{height},
      m_status{Status::Stopped},
      m_speed{f_defaultSpeed},
      m_generation{},
      m_livingCells{},
      m_deadCells{},
      m_mutex{} {}

Model::Status Model::status() const { return m_status; }

std::size_t Model::speed() const { return m_speed; }

std::size_t Model::width() const { return m_width; }

std::size_t Model::height() const { return m_height; }

std::size_t Model::generation() const { return m_generation; }

const std::set<Model::Cell>& Model::livingCells() {
  std::lock_guard<std::mutex> guard{m_mutex};
  return m_livingCells;
}

const std::set<Model::Cell>& Model::deadCells() {
  std::lock_guard<std::mutex> guard{m_mutex};
  return m_deadCells;
}

void Model::run() {
  if (m_status != Model::Status::Stopped) {
    m_status = Model::Status::Running;
    return;
  }
  m_status = Model::Status::Running;
  while (m_status != Model::Status::Stopped) {
    if (m_status == Model::Status::Running) {
      update();
    }
    std::this_thread::sleep_for(f_modelUpdatePeriod / m_speed);
  }
}

void Model::pause() { m_status = Model::Status::Paused; }

void Model::stop() { m_status = Model::Status::Stopped; }

void Model::increaseSpeed() { m_speed = std::min(f_maxSpeed, m_speed + 1); }

void Model::decreaseSpeed() { m_speed = std::max(f_minSpeed, m_speed - 1); }

void Model::addLivingCell(Cell position) {
  std::lock_guard<std::mutex> guard{m_mutex};
  m_livingCells.insert(position);
  m_deadCells.erase(position);
}

void Model::removeLivingCell(Cell position) {
  std::lock_guard<std::mutex> guard{m_mutex};
  m_livingCells.erase(position);
  if (m_livingCells.empty() && m_deadCells.empty()) {
    reset();
  }
}

void Model::generateLivingCells(std::size_t count) {
  std::lock_guard<std::mutex> guard{m_mutex};
  for (std::size_t i = 0; i < count; i++) {
    auto emptyPosCount{m_width * m_height - m_livingCells.size()};
    auto randomEmptyPosIndex{generateRandomValue(0, emptyPosCount)};
    std::size_t index{0};
    for (std::size_t x = 0; x < m_width; x++) {
      for (std::size_t y = 0; y < m_height; y++) {
        Cell cell{x, y};
        if (m_livingCells.count(cell) == 0) {
          if (index == randomEmptyPosIndex) {
            m_livingCells.insert(cell);
            x = m_width;
            y = m_height;
            break;
          }
          index++;
        }
      }
    }
  }
}

void Model::reset() {
  m_livingCells.clear();
  m_deadCells.clear();
  m_generation = 0;
}

void Model::update() {
  std::lock_guard<std::mutex> guard{m_mutex};
  auto updatedSurvivingCells{m_livingCells};
  std::map<Cell, int> deadCellsAndSurvivingNeighboursCount;
  for (const auto& cell : m_livingCells) {
    auto survivingNeighboursCount{0};
    for (auto x = cell.first - 1; x <= cell.first + 1; x++) {
      if (x < m_width) {
        for (auto y = cell.second - 1; y <= cell.second + 1; y++) {
          if (y < m_height) {
            Cell neighbour{x, y};
            if (neighbour != cell) {
              if (m_livingCells.count(neighbour) == 0) {
                deadCellsAndSurvivingNeighboursCount[neighbour]++;
              } else {
                survivingNeighboursCount++;
              }
            }
          }
        }
      }
    }
    if (survivingNeighboursCount < f_underpopulationThreshold ||
        survivingNeighboursCount > f_overpopulationThreshold) {
      updatedSurvivingCells.erase(cell);
      m_deadCells.insert(cell);
    }
  }
  for (const auto& value : deadCellsAndSurvivingNeighboursCount) {
    if (value.second == f_reproductionValue) {
      updatedSurvivingCells.insert({value.first});
      m_deadCells.erase(value.first);
    }
  }
  if (m_livingCells == updatedSurvivingCells) {
    m_status = Status::Paused;
    return;
  }
  m_livingCells = std::move(updatedSurvivingCells);
  m_generation++;
}
