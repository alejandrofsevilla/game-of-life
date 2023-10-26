#include "Model.hpp"

#include <algorithm>
#include <map>
#include <random>
#include <thread>

namespace {
constexpr auto f_underpopulationThreshold{2};
constexpr auto f_overpopulationThreshold{3};
constexpr auto f_reproductionValue{3};
constexpr auto f_modelUpdatePeriod{std::chrono::milliseconds{500}};
constexpr auto f_defaultSpeed{5};
constexpr auto f_maxSpeed{10};
constexpr auto f_minSpeed{1};
constexpr auto f_defaultSize{1};
constexpr auto f_maxSize{5};
constexpr auto f_minSize{1};

inline int generateRandomValue(int min, int max) {
  std::random_device rd;
  std::mt19937 gen{rd()};
  std::uniform_int_distribution<int> distr{min, max};
  return distr(gen);
}
}  // namespace

Model::Model(int maxWidth, int maxHeight)
    : m_maxWidth{maxWidth},
      m_maxHeight{maxHeight},
      m_status{Status::Uninitialized},
      m_size{f_defaultSize},
      m_width{calculateWidth()},
      m_height{calculateHeight()},
      m_speed{f_defaultSpeed},
      m_generation{},
      m_initialPattern{},
      m_cells{},
      m_timer{},
      m_mutex{} {}

Model::Status Model::status() const { return m_status; }

int Model::speed() const { return m_speed; }

int Model::maxSpeed() const { return f_maxSpeed; }

int Model::size() const { return m_size; }

int Model::maxSize() const { return f_maxSize; }

int Model::width() const { return m_width; }

int Model::height() const { return m_height; }

int Model::generation() const { return m_generation; }

std::set<Cell> Model::cells() {
  std::lock_guard guard(m_mutex);
  return m_cells;
}

const std::set<Cell>& Model::initialPattern() const { return m_initialPattern; }

void Model::run() {
  switch (m_status) {
    default:
      return;
    case Model::Status::Stopped:
      m_status = Model::Status::Running;
      return;
    case Model::Status::Paused:
      m_status = Model::Status::Running;
      return;
    case Model::Status::Uninitialized:
      m_status = Status::Stopped;
      while (m_status != Model::Status::Finished) {
        if (m_status == Model::Status::Running) {
          m_timer = std::async(&std::this_thread::sleep_for<int, std::milli>,
                               f_modelUpdatePeriod / m_speed);
          update();
          m_timer.wait();
        }
      }
      return;
  }
}

void Model::reset() {
  if (m_status == Model::Status::Stopped) {
    return;
  }
  m_cells = m_initialPattern;
  m_generation = 0;
  m_status = Status::Stopped;
}

void Model::clear() {
  m_cells.clear();
  m_initialPattern.clear();
  m_generation = 0;
  m_status = Status::Stopped;
}

void Model::pause() { m_status = Model::Status::Paused; }

void Model::finish() { m_status = Model::Status::Finished; }

void Model::speedUp() { m_speed = std::min(f_maxSpeed, m_speed + 1); }

void Model::slowDown() { m_speed = std::max(f_minSpeed, m_speed - 1); }

void Model::increaseSize() {
  if (!m_cells.empty()) {
    return;
  }
  m_size = std::min(f_maxSize, m_size + 1);
  m_width = calculateWidth();
  m_height = calculateHeight();
}

void Model::reduceSize() {
  if (!m_cells.empty()) {
    return;
  }
  m_size = std::max(f_minSize, m_size - 1);
  m_width = calculateWidth();
  m_height = calculateHeight();
}

void Model::insertPattern(const std::set<Cell>& pattern) {
  auto mostRightElement{
      std::max_element(pattern.cbegin(), pattern.cend(),
                       [](const auto& a, const auto& b) { return a.x < b.x; })};
  auto mostBottomElement{
      std::max_element(pattern.cbegin(), pattern.cend(),
                       [](const auto& a, const auto& b) { return a.y < b.y; })};
  auto width{mostRightElement->x};
  auto height{mostBottomElement->y};
  while (width > m_width || height > m_height) {
    increaseSize();
    if (m_size >= f_maxSize) {
      break;
    }
  }
  for (auto cell : pattern) {
    cell.x += (m_width - width) / 2;
    cell.y += (m_height - height) / 2;
    m_cells.insert(cell);
    m_initialPattern.insert(cell);
  }
}

void Model::insertCell(const Cell& cell) {
  if (cell.x > m_width || cell.y > m_height) {
    return;
  }
  m_cells.insert(cell);
  m_initialPattern.insert(cell);
}

void Model::removeCell(const Cell& cell) {
  if (cell.x > m_width || cell.y > m_height) {
    return;
  }
  m_cells.erase(cell);
  m_initialPattern.erase(cell);
}

void Model::generatePopulation(double density) {
  if (m_status != Model::Status::Stopped) {
    return;
  }
  auto population{m_width * m_height * density};
  for (int i = 0; i < population; i++) {
    auto pos{generateRandomValue(0, m_width * m_height)};
    Cell cell{pos % m_width, pos / m_width};
    insertCell(cell);
  }
}

void Model::update() {
  // see: https://en.wikipedia.org/wiki/Conway's_Game_of_Life#Rules
  std::lock_guard guard(m_mutex);
  auto updatedCells{m_cells};
  auto isUpdated{false};
  std::map<Cell, int> deadCellsWithAliveNeighboursCount;
  for (const auto& cell : m_cells) {
    if (cell.status == Cell::Status::Dead) {
      continue;
    }
    auto aliveNeighboursCount{0};
    for (auto x = cell.x - 1; x <= cell.x + 1; x++) {
      if (x < 0 || x >= m_width) {
        continue;
      }
      for (auto y = cell.y - 1; y <= cell.y + 1; y++) {
        if (y < 0 || y >= m_height) {
          continue;
        }
        Cell neighbour{x, y};
        if (neighbour == cell) {
          continue;
        }
        auto match{m_cells.find(neighbour)};
        if (match == m_cells.end() || match->status == Cell::Status::Dead) {
          deadCellsWithAliveNeighboursCount[neighbour]++;
        } else {
          aliveNeighboursCount++;
        }
      }
    }
    if (aliveNeighboursCount < f_underpopulationThreshold ||
        aliveNeighboursCount > f_overpopulationThreshold) {
      updatedCells.erase(cell);
      updatedCells.insert({cell.x, cell.y, Cell::Status::Dead});
      isUpdated = true;
    }
  }
  for (const auto& value : deadCellsWithAliveNeighboursCount) {
    if (value.second != f_reproductionValue) {
      continue;
    }
    auto cell{value.first};
    updatedCells.erase(cell);
    updatedCells.insert(cell);
    isUpdated = true;
  }
  if (m_cells.empty()) {
    m_status = Status::Stopped;
  } else if (!isUpdated) {
    m_status = Status::Paused;
  } else {
    m_cells = std::move(updatedCells);
    m_generation++;
  }
}

int Model::calculateWidth() {
  return m_maxWidth / std::max(1, 2 * (f_maxSize - m_size));
}

int Model::calculateHeight() {
  return m_maxHeight / std::max(1, 2 * (f_maxSize - m_size));
}
