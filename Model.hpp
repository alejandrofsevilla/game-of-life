#ifndef GAME_OF_LIFE_MODEL_HPP
#define GAME_OF_LIFE_MODEL_HPP

#include <set>
#include <vector>

#include "Cell.hpp"

class Model {
 public:
  enum class Status { ReadyToRun, Running, Paused, Stopped };

  Model(size_t maxWidth, size_t maxHeight);

  Status status() const;
  size_t speed() const;
  size_t maxSpeed() const;
  size_t size() const;
  size_t maxSize() const;
  size_t width() const;
  size_t height() const;
  size_t generation() const;
  const std::set<Cell>& aliveCells() const;
  const std::set<Cell>& deadCells() const;
  const std::set<Cell>& initialPattern() const;
  const std::set<size_t>& survivalRule() const;
  const std::set<size_t>& birthRule() const;

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
  void insertCell(const Cell& cell);
  void removeCell(const Cell& cell);
  void insertPattern(const std::set<Cell>& pattern);
  void setSurvivalRule(const std::set<size_t>& rule);
  void setBirthRule(const std::set<size_t>& rule);

 private:
  void updateStatus();
  void setSize(size_t size);

  size_t calculateWidth();
  size_t calculateHeight();

  const size_t m_maxWidth;
  const size_t m_maxHeight;

  Status m_status;
  size_t m_size;
  size_t m_width;
  size_t m_height;
  size_t m_speed;
  size_t m_generation;
  std::set<Cell> m_initialPattern;
  std::set<size_t> m_survivalRule;
  std::set<size_t> m_birthRule;
  std::set<Cell> m_aliveCells;
  std::set<Cell> m_deadCells;
  std::vector<std::vector<Cell::Status>> m_cellStatus;
};

#endif
