#ifndef GAME_OF_LIFE_MODEL_HPP
#define GAME_OF_LIFE_MODEL_HPP

#include <set>
#include <vector>

#include "Cell.hpp"

class Model {
public:
  enum class Status { ReadyToRun, Running, Paused, Stopped };

  Model(std::size_t maxWidth, std::size_t maxHeight);

  Status status() const;
  std::size_t speed() const;
  std::size_t minSpeed() const;
  std::size_t maxSpeed() const;
  std::size_t size() const;
  std::size_t maxSize() const;
  std::size_t width() const;
  std::size_t height() const;
  std::size_t generation() const;
  std::size_t population() const;
  const std::set<Cell> &initialPattern() const;
  const std::set<std::size_t> &survivalRule() const;
  const std::set<std::size_t> &birthRule() const;
  const Cell::Status &cellStatus(std::size_t col, std::size_t row) const;

  void update();
  void run();
  void pause();
  void clear();
  void reset();
  void speedUp();
  void slowDown();
  void increaseSize();
  void reduceSize();
  void generatePopulation(double density);
  void insertCell(const Cell &cell);
  void removeCell(const Cell &cell);
  void insertPattern(const std::set<Cell> &pattern);
  void setSurvivalRule(const std::set<std::size_t> &rule);
  void setBirthRule(const std::set<std::size_t> &rule);

private:
  void updateStatus();
  void setSize(std::size_t size);

  std::size_t updateSection(std::size_t minCol, std::size_t maxCol);

  std::size_t calculateWidth();
  std::size_t calculateHeight();

  const std::size_t m_maxWidth;
  const std::size_t m_maxHeight;

  Status m_status;
  std::size_t m_size;
  std::size_t m_width;
  std::size_t m_height;
  std::size_t m_speed;
  std::size_t m_generation;
  std::size_t m_population;
  std::set<Cell> m_initialPattern;
  std::set<std::size_t> m_survivalRule;
  std::set<std::size_t> m_birthRule;
  std::vector<std::vector<Cell::Status>> m_cellStatus;
  std::vector<std::vector<Cell::Status>> m_updatedCellStatus;
};

#endif
