#include "Model.hpp"

#include <algorithm>
#include <random>

#include "Cell.hpp"

namespace {
constexpr size_t f_underpopulationThreshold{2};
constexpr size_t f_overpopulationThreshold{3};
constexpr size_t f_reproductionValue{3};
constexpr size_t f_defaultSpeed{10};
constexpr size_t f_maxSpeed{10};
constexpr size_t f_minSpeed{1};
constexpr size_t f_defaultSize{5};
constexpr size_t f_maxSize{5};
constexpr size_t f_minSize{1};
constexpr size_t f_minRuleValue{0};
constexpr size_t f_maxRuleValue{8};
constexpr std::initializer_list<size_t> f_conwaysBirthRule{3};
constexpr std::initializer_list<size_t> f_conwaysSurvivalRule{2, 3};

inline size_t generateRandomValue(size_t min, size_t max) {
  std::random_device rd;
  std::mt19937 gen{rd()};
  std::uniform_int_distribution<size_t> distr{min, max};
  return distr(gen);
}
}  // namespace

Model::Model(size_t maxWidth, size_t maxHeight)
    : m_maxWidth{maxWidth},
      m_maxHeight{maxHeight},
      m_status{Status::Stopped},
      m_size{f_defaultSize},
      m_width{calculateWidth()},
      m_height{calculateHeight()},
      m_speed{f_defaultSpeed},
      m_generation{},
      m_initialPattern{},
      m_survivalRule{f_conwaysSurvivalRule},
      m_birthRule{f_conwaysBirthRule},
      m_aliveCells{},
      m_deadCells{},
      m_cellStatus{static_cast<size_t>(m_width),
                   {static_cast<size_t>(m_height), Cell::Status::Empty}} {}

Model::Status Model::status() const { return m_status; }

size_t Model::speed() const { return m_speed; }

size_t Model::maxSpeed() const { return f_maxSpeed; }

size_t Model::size() const { return m_size; }

size_t Model::maxSize() const { return f_maxSize; }

size_t Model::width() const { return m_width; }

size_t Model::height() const { return m_height; }

size_t Model::generation() const { return m_generation; }

const std::set<Cell>& Model::aliveCells() const { return m_aliveCells; }

const std::set<Cell>& Model::deadCells() const { return m_deadCells; }

const std::set<Cell>& Model::initialPattern() const { return m_initialPattern; }

const std::set<size_t>& Model::survivalRule() const { return m_survivalRule; }

const std::set<size_t>& Model::birthRule() const { return m_birthRule; }

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

void Model::pause() { m_status = Model::Status::Paused; }

void Model::reset() {
  std::fill(m_cellStatus.begin(), m_cellStatus.end(),
            std::vector<Cell::Status>{static_cast<std::size_t>(m_height),
                                      Cell::Status::Empty});
  m_aliveCells = m_initialPattern;
  for (const auto& cell : m_aliveCells) {
    m_cellStatus[cell.x][cell.y] = Cell::Status::Alive;
  }
  m_deadCells.clear();
  m_generation = 0;
  updateStatus();
}

void Model::clear() {
  std::fill(m_cellStatus.begin(), m_cellStatus.end(),
            std::vector<Cell::Status>{static_cast<std::size_t>(m_height),
                                      Cell::Status::Empty});
  m_aliveCells.clear();
  m_deadCells.clear();
  m_initialPattern.clear();
  m_generation = 0;
  updateStatus();
}

void Model::speedUp() { m_speed = std::min(f_maxSpeed, m_speed + 1); }

void Model::slowDown() { m_speed = std::max(f_minSpeed, m_speed - 1); }

void Model::increaseSize() { setSize(m_size + 1); }

void Model::reduceSize() { setSize(m_size - 1); }

void Model::insertCell(const Cell& cell) {
  m_cellStatus[static_cast<std::size_t>(cell.x)]
              [static_cast<std::size_t>(cell.y)] = Cell::Status::Alive;
  m_aliveCells.insert(cell);
  m_deadCells.erase(cell);
  m_initialPattern.insert(cell);
  updateStatus();
}

void Model::removeCell(const Cell& cell) {
  m_cellStatus[static_cast<std::size_t>(cell.x)]
              [static_cast<std::size_t>(cell.y)] = Cell::Status::Empty;
  m_aliveCells.erase(cell);
  m_deadCells.erase(cell);
  m_initialPattern.erase(cell);
  updateStatus();
}

void Model::insertPattern(const std::set<Cell>& pattern) {
  auto mostLeftElement{
      std::min_element(pattern.cbegin(), pattern.cend(),
                       [](const auto& a, const auto& b) { return a.x < b.x; })};
  auto mostTopElement{
      std::min_element(pattern.cbegin(), pattern.cend(),
                       [](const auto& a, const auto& b) { return a.y < b.y; })};
  auto mostRightElement{
      std::max_element(pattern.cbegin(), pattern.cend(),
                       [](const auto& a, const auto& b) { return a.x < b.x; })};
  auto mostBottomElement{
      std::max_element(pattern.cbegin(), pattern.cend(),
                       [](const auto& a, const auto& b) { return a.y < b.y; })};
  auto width{static_cast<size_t>(mostRightElement->x - mostLeftElement->x)};
  auto height{static_cast<size_t>(mostBottomElement->y - mostTopElement->y)};
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

void Model::setBirthRule(const std::set<size_t>& rule) {
  m_birthRule.clear();
  for (auto val : rule) {
    m_birthRule.insert(std::max(std::min(val, f_maxRuleValue), f_minRuleValue));
  }
}

void Model::setSurvivalRule(const std::set<size_t>& rule) {
  m_survivalRule.clear();
  for (auto val : rule) {
    m_survivalRule.insert(
        std::max(std::min(val, f_maxRuleValue), f_minRuleValue));
  }
}

void Model::generatePopulation(double density) {
  auto population{static_cast<double>(m_width * m_height) * density};
  for (size_t i = 0; i < static_cast<size_t>(population); i++) {
    auto pos{generateRandomValue(0, m_width * m_height)};
    Cell cell{pos % m_width, pos / m_width};
    insertCell(cell);
  }
  updateStatus();
}

void Model::update() {
  std::vector<Cell> bornCells;
  std::vector<Cell> dyingCells;
  bool isUpdated{false};
  for (std::size_t col = 0; col < static_cast<std::size_t>(m_width); col++) {
    for (std::size_t row = 0; row < static_cast<std::size_t>(m_height); row++) {
      auto numberOfAliveNeighbours{0};
      for (std::size_t x = col - 1; x <= col + 1; x++) {
        if (x >= static_cast<std::size_t>(m_width)) {
          continue;
        }
        for (std::size_t y = row - 1; y <= row + 1; y++) {
          if (y >= static_cast<std::size_t>(m_height) ||
              (x == col && y == row)) {
            continue;
          }
          if (m_cellStatus[x][y] == Cell::Status::Alive) {
            numberOfAliveNeighbours++;
          }
        }
      }
      Cell cell{col, row};
      auto status{m_cellStatus[col][row]};
      if (status == Cell::Status::Alive &&
          m_survivalRule.count(numberOfAliveNeighbours) == 0) {
        dyingCells.push_back(cell);
        isUpdated = true;
      } else if (m_birthRule.count(numberOfAliveNeighbours) != 0) {
        bornCells.push_back(cell);
        isUpdated = true;
      }
    }
  }
  for (const auto& cell : bornCells) {
    m_cellStatus[cell.x][cell.y] = Cell::Status::Alive;
    m_aliveCells.insert(cell);
    m_deadCells.erase(cell);
  }
  for (const auto& cell : dyingCells) {
    m_cellStatus[cell.x][cell.y] = Cell::Status::Dead;
    m_deadCells.insert(m_aliveCells.extract(cell));
  }
  if (!isUpdated) {
    m_status = Status::Paused;
  }
  m_generation++;
}

void Model::updateStatus() {
  if (m_aliveCells.size() > 0) {
    m_status = Status::ReadyToRun;
  } else if (m_deadCells.size() == 0) {
    m_status = Status::Stopped;
  }
}

void Model::setSize(size_t size) {
  m_size = std::max(f_minSize, std::min(f_maxSize, size));
  m_width = calculateWidth();
  m_height = calculateHeight();
  m_cellStatus.resize(m_width);
  std::for_each(m_cellStatus.begin(), m_cellStatus.end(),
                [this](auto& row) { row.resize(m_height); });
}

size_t Model::calculateWidth() {
  return m_maxWidth / std::max(size_t{1}, 2 * (f_maxSize - m_size));
}

size_t Model::calculateHeight() {
  return m_maxHeight / std::max(size_t{1}, 2 * (f_maxSize - m_size));
}
