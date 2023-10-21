#include "Model.hpp"

#include <map>
#include <random>
#include <thread>

namespace {
constexpr auto f_underpopulationThreshold{2};
constexpr auto f_overpopulationThreshold{3};
constexpr auto f_reproductionValue{3};
constexpr auto f_defaultSize{10};
constexpr auto f_modelUpdatePeriod{std::chrono::milliseconds{1000}};
constexpr std::size_t f_defaultSpeed{5};
constexpr std::size_t f_maxSpeed{10};
constexpr std::size_t f_minSpeed{1};
constexpr std::size_t f_maxSize{20};
constexpr std::size_t f_minSize{1};

inline std::size_t generateRandomValue(std::size_t min, std::size_t max) {
  std::random_device rd;
  std::mt19937 gen{rd()};
  std::uniform_int_distribution<std::size_t> distr{min, max};
  return distr(gen);
}
}  // namespace

Model::Model(std::size_t maxWidth, std::size_t maxHeight)
    : m_maxWidth{maxWidth},
      m_maxHeight{maxHeight},
      m_status{Status::Uninitialized},
      m_size{f_defaultSize},
      m_width{calculateWidth()},
      m_height{calculateHeight()},
      m_speed{f_defaultSpeed},
      m_generation{},
      m_pattern{},
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
  switch (m_status) {
    default:
      return;
    case Model::Status::Stopped:
      m_pattern = m_livingCells;
      m_status = Model::Status::Running;
      return;
    case Model::Status::Paused:
      m_status = Model::Status::Running;
      return;
    case Model::Status::Uninitialized:
      m_status = Status::Stopped;
      while (m_status != Model::Status::Finished) {
        if (m_status == Model::Status::Running) {
          update();
        }
        std::this_thread::sleep_for(f_modelUpdatePeriod / m_speed);
      }
      return;
  }
}

void Model::reset() {
  if (m_status == Model::Status::Stopped) {
    return;
  }
  clear();
  m_livingCells = m_pattern;
}

void Model::clear() {
  m_livingCells.clear();
  m_deadCells.clear();
  m_generation = 0;
  m_status = Status::Stopped;
}

void Model::pause() { m_status = Model::Status::Paused; }

void Model::finish() { m_status = Model::Status::Finished; }

void Model::speedUp() { m_speed = std::min(f_maxSpeed, m_speed + 1); }

void Model::slowDown() { m_speed = std::max(f_minSpeed, m_speed - 1); }

void Model::increaseSize() {
  if (m_status != Model::Status::Stopped || !m_livingCells.empty()) {
    return;
  }
  m_size = std::min(f_maxSize, m_size + 1);
  m_width = calculateWidth();
  m_height = calculateHeight();
}

void Model::reduceSize() {
  if (m_status != Model::Status::Stopped || !m_livingCells.empty()) {
    return;
  }
  m_size = std::max(f_minSize, m_size - 1);
  m_width = calculateWidth();
  m_height = calculateHeight();
}

void Model::addLivingCell(Cell position) {
  if (m_status != Model::Status::Stopped) {
    return;
  }
  std::lock_guard<std::mutex> guard{m_mutex};
  m_livingCells.insert(position);
}

void Model::removeLivingCell(Cell position) {
  if (m_status != Model::Status::Stopped) {
    return;
  }
  std::lock_guard<std::mutex> guard{m_mutex};
  m_livingCells.erase(position);
}

void Model::generatePopulation(double populationRate) {
  if (m_status != Model::Status::Stopped) {
    return;
  }
  std::lock_guard<std::mutex> guard{m_mutex};
  auto population{static_cast<size_t>(static_cast<double>(m_width) *
                                      static_cast<double>(m_height) *
                                      populationRate)};
  for (std::size_t i = 0; i < population; i++) {
    auto isEmptyCell{ false };
    auto pos{ generateRandomValue(0, m_width * m_height) };
    Cell cell{ pos%m_width, pos / m_width };
    if (m_livingCells.count(cell) == 0) {
      m_livingCells.insert(cell);
      isEmptyCell = true;
    }
   }
}

void Model::update() {
  // see: https://en.wikipedia.org/wiki/Conway's_Game_of_Life#Rules
  std::lock_guard<std::mutex> guard{m_mutex};
  auto updatedSurvivingCells{m_livingCells};
  std::map<Cell, int> deadCellsAndSurvivingNeighboursCount;
  for (const auto& cell : m_livingCells) {
    auto survivingNeighboursCount{0};
    for (auto x = cell.first - 1; x <= cell.first + 1; x++) {
      if (x >= m_width) {
        continue;
      }
      for (auto y = cell.second - 1; y <= cell.second + 1; y++) {
        if (y >= m_height) {
          continue;
        }
        Cell neighbour{x, y};
        if (neighbour == cell) {
          continue;
        }
        if (m_livingCells.count(neighbour) == 0) {
          deadCellsAndSurvivingNeighboursCount[neighbour]++;
        } else {
          survivingNeighboursCount++;
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
    if (value.second != f_reproductionValue) {
      continue;
    }
    updatedSurvivingCells.insert({value.first});
    m_deadCells.erase(value.first);
  }
  if (m_livingCells.empty() && m_deadCells.empty()) {
    m_status = Status::Stopped;
  } else if (m_livingCells == updatedSurvivingCells) {
    m_status = Status::Paused;
  } else {
    m_livingCells = std::move(updatedSurvivingCells);
    m_generation++;
  }
}

std::size_t Model::calculateWidth() {
  return static_cast<std::size_t>(static_cast<double>(m_maxWidth * m_size) /
                                  f_maxSize);
}

std::size_t Model::calculateHeight() {
  return static_cast<std::size_t>(static_cast<double>(m_maxHeight * m_size) /
                                  f_maxSize);
}
