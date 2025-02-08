#include "Model.hpp"

#include <algorithm>
#include <random>

#include "Cell.hpp"

namespace {
constexpr size_t f_defaultSpeed{10};
constexpr size_t f_maxSpeed{10};
constexpr size_t f_minSpeed{1};
constexpr size_t f_maxSize{5};
constexpr size_t f_minSize{1};
constexpr size_t f_minRuleValue{0};
constexpr size_t f_maxRuleValue{8};
constexpr std::initializer_list<size_t> f_conwaysBirthRule{3};
constexpr std::initializer_list<size_t> f_conwaysSurvivalRule{2, 3};

inline auto generateRandomValue(size_t min, size_t max) {
  std::random_device rd;
  std::mt19937 gen{rd()};
  std::uniform_int_distribution<size_t> distr{min, max};
  return distr(gen);
}
inline auto generateCells(size_t width, size_t height) {
  std::vector<Cell> cells;
  for (size_t row = 0; row < height; row++) {
    for (size_t col = 0; col < width; col++) {
      cells.push_back({col, row, Cell::Status::Empty});
    }
  }
  return cells;
}
} // namespace

Model::Model(size_t width, size_t height)
    : m_width{width}, m_height{height}, m_status{Status::Stopped},
      m_speed{f_defaultSpeed}, m_generation{}, m_population{},
      m_initialPattern{}, m_survivalRule{f_conwaysSurvivalRule},
      m_birthRule{f_conwaysBirthRule},
      m_cellStatus{m_width, {m_height, Cell::Status::Empty}},
      m_updatedCellStatus{m_cellStatus}, m_cells{generateCells(width, height)} {
}

Model::Status Model::status() const { return m_status; }

size_t Model::speed() const { return m_speed; }

size_t Model::minSpeed() const { return f_minSpeed; }

size_t Model::maxSpeed() const { return f_maxSpeed; }

size_t Model::width() const { return m_width; }

size_t Model::height() const { return m_height; }

size_t Model::generation() const { return m_generation; }

size_t Model::population() const { return m_population; }

std::optional<Cell> Model::cell(std::size_t col, std::size_t row) const {
  auto index{toCellIndex(col, row)};
  return (index < m_cells.size()) ? m_cells.at(index) : std::optional<Cell>{};
}

const std::set<Cell> &Model::initialPattern() const { return m_initialPattern; }

const std::set<size_t> &Model::survivalRule() const { return m_survivalRule; }

const std::set<size_t> &Model::birthRule() const { return m_birthRule; }

const std::vector<Cell> &Model::cells() const { return m_cells; }

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
  m_generation = 0;
  m_population = 0;
  std::for_each(m_cells.begin(), m_cells.end(),
                [](auto &c) { c.status = Cell::Status::Empty; });
  for (const auto &cell : m_initialPattern) {
    m_cells.at(toCellIndex(cell.col, cell.row)).status = Cell::Status::Alive;
    m_population++;
  }
  updateStatus();
}

void Model::clear() {
  m_generation = 0;
  m_population = 0;
  std::for_each(m_cells.begin(), m_cells.end(),
                [](auto &c) { c.status = Cell::Status::Empty; });
  m_initialPattern.clear();
  updateStatus();
}

void Model::speedUp() { m_speed = std::min(f_maxSpeed, m_speed + 1); }

void Model::slowDown() { m_speed = std::max(f_minSpeed, m_speed - 1); }

void Model::insertCell(const Cell &cell) {
  m_cells.at(toCellIndex(cell.col, cell.row)).status = Cell::Status::Alive;
  m_initialPattern.insert(cell);
  m_population++;
  updateStatus();
}

void Model::removeCell(const Cell &cell) {
  m_cells.at(toCellIndex(cell.col, cell.row)).status = Cell::Status::Empty;
  m_initialPattern.erase(cell);
  m_population--;
  updateStatus();
}

void Model::insertPattern(const std::set<Cell> &pattern) {
  auto mostLeftElement{std::min_element(
      pattern.cbegin(), pattern.cend(),
      [](const auto &a, const auto &b) { return a.col < b.col; })};
  auto mostTopElement{std::min_element(
      pattern.cbegin(), pattern.cend(),
      [](const auto &a, const auto &b) { return a.row < b.row; })};
  auto mostRightElement{std::max_element(
      pattern.cbegin(), pattern.cend(),
      [](const auto &a, const auto &b) { return a.col < b.col; })};
  auto mostBottomElement{std::max_element(
      pattern.cbegin(), pattern.cend(),
      [](const auto &a, const auto &b) { return a.row < b.row; })};
  auto width{mostRightElement->col - mostLeftElement->col};
  auto height{mostBottomElement->row - mostTopElement->row};
  for (auto cell : pattern) {
    cell.col += (m_width - width) / 2;
    cell.row += (m_height - height) / 2;
    m_initialPattern.insert(cell);
    m_cells.at(toCellIndex(cell.col, cell.row)).status = Cell::Status::Alive;
    m_population++;
    updateStatus();
  }
}

void Model::setBirthRule(const std::set<size_t> &rule) {
  m_birthRule.clear();
  for (auto val : rule) {
    m_birthRule.insert(std::max(std::min(val, f_maxRuleValue), f_minRuleValue));
  }
}

void Model::setSurvivalRule(const std::set<size_t> &rule) {
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

std::size_t Model::toCellIndex(std::size_t col, std::size_t row) const {
  return row * m_width + col;
}

void Model::update() {
  auto cells{m_cells};
  auto population{0};
  std::transform(m_cells.cbegin(), m_cells.cend(), cells.begin(),
                 [this, &population](auto cell) {
                   auto numberOfAliveNeighbours{0};
                   for (auto col = cell.col - 1; col <= cell.col + 1; col++) {
                     if (col >= m_width) {
                       continue;
                     }
                     for (auto row = cell.row - 1; row <= cell.row + 1; row++) {
                       if (row >= m_height ||
                           (col == cell.col && row == cell.row)) {
                         continue;
                       }
                       if (m_cells.at(toCellIndex(col, row)).status ==
                           Cell::Status::Alive) {
                         numberOfAliveNeighbours++;
                       }
                     }
                   }
                   if (cell.status == Cell::Status::Alive &&
                       m_survivalRule.count(numberOfAliveNeighbours) == 0) {
                     cell.status = Cell::Status::Dead;
                   } else if (m_birthRule.count(numberOfAliveNeighbours) != 0) {
                     cell.status = Cell::Status::Alive;
                   }
                   if (cell.status == Cell::Status::Alive) {
                     population++;
                   }
                   return cell;
                 });
  m_cells = std::move(cells);
  m_population = population;
  m_generation++;
}

void Model::updateStatus() {
  if (m_population > 0) {
    m_status = Status::ReadyToRun;
  } else if (m_population == 0) {
    m_status = Status::Stopped;
  }
}