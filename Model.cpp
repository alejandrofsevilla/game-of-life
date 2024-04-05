#include "Model.hpp"

#include <algorithm>
#include <future>
#include <random>
#include <thread>

#include "Cell.hpp"

namespace {
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
      m_population{},
      m_initialPattern{},
      m_survivalRule{f_conwaysSurvivalRule},
      m_birthRule{f_conwaysBirthRule},
      m_cellStatus{m_width, {m_height, Cell::Status::Empty}},
      m_updatedCellStatus{m_cellStatus} {}

Model::Status Model::status() const { return m_status; }

size_t Model::speed() const { return m_speed; }

size_t Model::maxSpeed() const { return f_maxSpeed; }

size_t Model::size() const { return m_size; }

size_t Model::maxSize() const { return f_maxSize; }

size_t Model::width() const { return m_width; }

size_t Model::height() const { return m_height; }

size_t Model::generation() const { return m_generation; }

size_t Model::population() const { return m_population; }

const std::set<Cell>& Model::initialPattern() const { return m_initialPattern; }

const std::set<size_t>& Model::survivalRule() const { return m_survivalRule; }

const std::set<size_t>& Model::birthRule() const { return m_birthRule; }

const Cell::Status& Model::cellStatus(std::size_t col, std::size_t row) const {
  return m_cellStatus[col][row];
}

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
            std::vector<Cell::Status>{m_height, Cell::Status::Empty});
  m_population = 0;
  for (const auto& cell : m_initialPattern) {
    m_cellStatus[cell.col][cell.row] = Cell::Status::Alive;
    m_population++;
  }
  m_generation = 0;
  updateStatus();
}

void Model::clear() {
  std::fill(m_cellStatus.begin(), m_cellStatus.end(),
            std::vector<Cell::Status>{m_height, Cell::Status::Empty});
  m_initialPattern.clear();
  m_generation = 0;
  m_population = 0;
  updateStatus();
}

void Model::speedUp() { m_speed = std::min(f_maxSpeed, m_speed + 1); }

void Model::slowDown() { m_speed = std::max(f_minSpeed, m_speed - 1); }

void Model::increaseSize() { setSize(m_size + 1); }

void Model::reduceSize() { setSize(m_size - 1); }

void Model::insertCell(const Cell& cell) {
  m_cellStatus[cell.col][cell.row] = Cell::Status::Alive;
  m_initialPattern.insert(cell);
  m_population++;
  updateStatus();
}

void Model::removeCell(const Cell& cell) {
  m_cellStatus[cell.col][cell.row] = Cell::Status::Empty;
  m_initialPattern.erase(cell);
  m_population--;
  updateStatus();
}

void Model::insertPattern(const std::set<Cell>& pattern) {
  auto mostLeftElement{std::min_element(
      pattern.cbegin(), pattern.cend(),
      [](const auto& a, const auto& b) { return a.col < b.col; })};
  auto mostTopElement{std::min_element(
      pattern.cbegin(), pattern.cend(),
      [](const auto& a, const auto& b) { return a.row < b.row; })};
  auto mostRightElement{std::max_element(
      pattern.cbegin(), pattern.cend(),
      [](const auto& a, const auto& b) { return a.col < b.col; })};
  auto mostBottomElement{std::max_element(
      pattern.cbegin(), pattern.cend(),
      [](const auto& a, const auto& b) { return a.row < b.row; })};
  auto width{mostRightElement->col - mostLeftElement->col};
  auto height{mostBottomElement->row - mostTopElement->row};
  while (width > m_width || height > m_height) {
    increaseSize();
    if (m_size >= f_maxSize) {
      break;
    }
  }
  for (auto cell : pattern) {
    cell.col += (m_width - width) / 2;
    cell.row += (m_height - height) / 2;
    m_initialPattern.insert(cell);
    m_cellStatus[cell.col][cell.row] = Cell::Status::Alive;
    m_population++;
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
    auto pos{generateRandomValue(0, m_width * m_height - 1)};
    Cell cell{pos % m_width, pos / m_width};
    insertCell(cell);
  }
  updateStatus();
}

std::size_t Model::updateSection(std::size_t minCol, std::size_t maxCol) {
  std::size_t population{0};
  for (std::size_t col = minCol; col < maxCol; col++) {
    for (std::size_t row = 0; row < m_height; row++) {
      std::size_t numberOfAliveNeighbours{0};
      for (std::size_t x = col - 1; x <= col + 1; x++) {
        if (x >= m_width) {
          continue;
        }
        for (std::size_t y = row - 1; y <= row + 1; y++) {
          if (y >= m_height || (x == col && y == row)) {
            continue;
          }
          if (m_cellStatus[x][y] == Cell::Status::Alive) {
            numberOfAliveNeighbours++;
          }
        }
      }
      if (m_cellStatus[col][row] == Cell::Status::Alive &&
          m_survivalRule.count(numberOfAliveNeighbours) == 0) {
        m_updatedCellStatus[col][row] = Cell::Status::Dead;
      } else if (m_birthRule.count(numberOfAliveNeighbours) != 0) {
        m_updatedCellStatus[col][row] = Cell::Status::Alive;
      } else {
        m_updatedCellStatus[col][row] = m_cellStatus[col][row];
      }
      if (m_updatedCellStatus[col][row] == Cell::Status::Alive) {
        population++;
      }
    }
  }
  return population;
}

void Model::update() {
  auto numOfThreads{std::thread::hardware_concurrency()};
  auto sectionsPerThread{static_cast<std::size_t>(
      std::ceil(static_cast<double>(m_width) / numOfThreads))};
  std::vector<std::future<std::size_t>> tasks;
  for (std::size_t i = 0; i < numOfThreads; i++) {
    auto begin{std::min(i * sectionsPerThread, m_width)};
    auto end{std::min((i + 1) * sectionsPerThread, m_width)};
    tasks.emplace_back(std::async(&Model::updateSection, this, begin, end));
  }
  m_population=0;
  for (auto& t : tasks) {
    m_population += t.get();
  }
  std::swap(m_cellStatus, m_updatedCellStatus);
  m_generation++;
}

void Model::updateStatus() {
  if (m_population > 0) {
    m_status = Status::ReadyToRun;
  } else if (m_population == 0) {
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
