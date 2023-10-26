#include "Model.hpp"

#include <algorithm>
#include <map>
#include <random>

namespace {
constexpr auto f_underpopulationThreshold{2};
constexpr auto f_overpopulationThreshold{3};
constexpr auto f_reproductionValue{3};
constexpr auto f_defaultSpeed{10};
constexpr auto f_maxSpeed{10};
constexpr auto f_minSpeed{1};
constexpr auto f_defaultSize{5};
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
      m_status{Status::Stopped},
      m_size{f_defaultSize},
      m_width{calculateWidth()},
      m_height{calculateHeight()},
      m_speed{f_defaultSpeed},
      m_generation{},
      m_initialPattern{},
      m_aliveCells{},
      m_deadCells{} {}

Model::Status Model::status() const { return m_status; }

int Model::speed() const { return m_speed; }

int Model::maxSpeed() const { return f_maxSpeed; }

int Model::size() const { return m_size; }

int Model::maxSize() const { return f_maxSize; }

int Model::width() const { return m_width; }

int Model::height() const { return m_height; }

int Model::generation() const { return m_generation; }

const std::set<Cell>& Model::aliveCells() { return m_aliveCells; }

const std::set<Cell>& Model::deadCells() { return m_deadCells; }

const std::set<Cell>& Model::initialPattern() const { return m_initialPattern; }

void Model::run() {
  switch (m_status) {
    case Model::Status::ReadyToRun:
      m_status = Model::Status::Running;
      return;
    case Model::Status::Paused:
      m_status = Model::Status::Running;
      return;
    default:
      return;
  }
}

void Model::reset() {
  m_aliveCells = m_initialPattern;
  m_deadCells.clear();
  m_generation = 0;
  updateStatus();
}

void Model::clear() {
  m_aliveCells.clear();
  m_deadCells.clear();
  m_initialPattern.clear();
  m_generation = 0;
  updateStatus();
}

void Model::pause() { m_status = Model::Status::Paused; }

void Model::finish() { m_status = Model::Status::Finished; }

void Model::speedUp() { m_speed = std::min(f_maxSpeed, m_speed + 1); }

void Model::slowDown() { m_speed = std::max(f_minSpeed, m_speed - 1); }

void Model::increaseSize() {
  m_size = std::min(f_maxSize, m_size + 1);
  m_width = calculateWidth();
  m_height = calculateHeight();
}

void Model::reduceSize() {
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
    m_aliveCells.insert(cell);
    m_initialPattern.insert(cell);
    updateStatus();
  }
}

void Model::insertCell(const Cell& cell) {
  if (cell.x > m_width || cell.y > m_height) {
    return;
  }
  m_aliveCells.insert(cell);
  m_deadCells.erase(cell);
  m_initialPattern.insert(cell);
  updateStatus();
}

void Model::removeCell(const Cell& cell) {
  if (cell.x > m_width || cell.y > m_height) {
    return;
  }
  m_aliveCells.erase(cell);
  m_deadCells.erase(cell);
  m_initialPattern.erase(cell);
  updateStatus();
}

void Model::generatePopulation(double density) {
  auto population{m_width * m_height * density};
  for (int i = 0; i < population; i++) {
    auto pos{generateRandomValue(0, m_width * m_height)};
    Cell cell{pos % m_width, pos / m_width};
    insertCell(cell);
  }
  updateStatus();
}

void Model::update() {
  // see: https://en.wikipedia.org/wiki/Conway's_Game_of_Life#Rules
  auto updatedAliveCells{m_aliveCells};
  auto isUpdated{false};
  std::map<Cell, int> deadCellsWithAliveNeighboursCount;
  for (const auto& cell : m_aliveCells) {
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
        if (m_aliveCells.count(neighbour) == 0) {
          deadCellsWithAliveNeighboursCount[neighbour]++;
        } else {
          aliveNeighboursCount++;
        }
      }
    }
    if (aliveNeighboursCount < f_underpopulationThreshold ||
        aliveNeighboursCount > f_overpopulationThreshold) {
      updatedAliveCells.erase(cell);
      m_deadCells.insert(cell);
      isUpdated = true;
    }
  }
  for (const auto& value : deadCellsWithAliveNeighboursCount) {
    if (value.second == f_reproductionValue) {
      m_deadCells.erase(value.first);
      updatedAliveCells.insert(value.first);
    }
    isUpdated = true;
  }
  if (!isUpdated) {
    m_status = Status::Paused;
    updateStatus();
  } else {
    m_aliveCells = std::move(updatedAliveCells);
    m_generation++;
  }
}

void Model::updateStatus() {
  if (m_aliveCells.size() > 0) {
    m_status = Status::ReadyToRun;
  } else if (m_deadCells.size() == 0) {
    m_status = Status::Stopped;
  }
}

int Model::calculateWidth() {
  return m_maxWidth / std::max(1, 2 * (f_maxSize - m_size));
}

int Model::calculateHeight() {
  return m_maxHeight / std::max(1, 2 * (f_maxSize - m_size));
}
