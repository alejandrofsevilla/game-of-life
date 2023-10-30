#ifndef GAME_OF_LIFE_MODEL_HPP
#define GAME_OF_LIFE_MODEL_HPP

#include <set>

#include "Cell.hpp"

class Model {
 public:
  enum class Status { ReadyToRun, Running, Paused, Stopped };

  Model(int maxWidth, int maxHeight);

  Status status() const;
  int speed() const;
  int maxSpeed() const;
  int size() const;
  int maxSize() const;
  int width() const;
  int height() const;
  int generation() const;
  const std::set<Cell>& aliveCells() const;
  const std::set<Cell>& deadCells() const;
  const std::set<Cell>& initialPattern() const;
  const std::set<int>& survivalRule() const;
  const std::set<int>& birthRule() const;

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
  void setSurvivalRule(const std::set<int>& rule);
  void setBirthRule(const std::set<int>& rule);

 private:
  void updateStatus();

  int calculateWidth();
  int calculateHeight();

  const int m_maxWidth;
  const int m_maxHeight;

  Status m_status;
  int m_size;
  int m_width;
  int m_height;
  int m_speed;
  int m_generation;
  std::set<Cell> m_initialPattern;
  std::set<Cell> m_aliveCells;
  std::set<Cell> m_deadCells;
  std::set<int> m_survivalRule;
  std::set<int> m_birthRule;
};

#endif
