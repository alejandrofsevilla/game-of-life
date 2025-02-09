#ifndef GAME_OF_LIFE_MODEL_HPP
#define GAME_OF_LIFE_MODEL_HPP

#include <set>
#include <vector>

#include "Cell.hpp"
#include <optional>

class Model {
public:
  enum class Status { ReadyToRun, Running, Paused, Stopped };

  Model(std::size_t width, std::size_t height);

  Status status() const;
  std::size_t speed() const;
  std::size_t minSpeed() const;
  std::size_t maxSpeed() const;
  std::size_t width() const;
  std::size_t height() const;
  std::size_t generation() const;
  std::size_t population() const;
  std::optional<Cell> cellAt(std::size_t col, std::size_t row) const;
  const std::set<Cell> &initialPattern() const;
  const std::set<std::size_t> &survivalRule() const;
  const std::set<std::size_t> &birthRule() const;
  const std::vector<Cell> &cells() const;

  void update();
  void run();
  void pause();
  void clear();
  void reset();
  void speedUp();
  void slowDown();
  void generatePopulation(double density);
  void insertCell(const Cell &cell);
  void removeCell(const Cell &cell);
  void insertPattern(const std::set<Cell> &pattern);
  void setSurvivalRule(const std::set<std::size_t> &rule);
  void setBirthRule(const std::set<std::size_t> &rule);

private:
  void updateStatus();

  std::size_t toCellIndex(std::size_t col, std::size_t row) const;

  const std::size_t m_width;
  const std::size_t m_height;

  Status m_status;
  std::size_t m_speed;
  std::size_t m_generation;
  std::size_t m_population;
  std::set<Cell> m_initialPattern;
  std::set<std::size_t> m_survivalRule;
  std::set<std::size_t> m_birthRule;
  std::vector<std::vector<Cell::Status>> m_cellStatus;
  std::vector<std::vector<Cell::Status>> m_updatedCellStatus;
  std::vector<Cell> m_cells;
};

#endif
