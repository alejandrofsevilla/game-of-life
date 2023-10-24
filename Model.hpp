#ifndef GAME_OF_LIFE_MODEL_HPP
#define GAME_OF_LIFE_MODEL_HPP

#include <map>
#include <mutex>
#include <set>

#include "Cell.hpp"

class Model {
 public:
  enum class Status { Uninitialized, Stopped, Running, Paused, Finished };

  Model(int maxWidth, int maxHeight);

  Status status() const;
  int speed() const;
  int width() const;
  int height() const;
  int generation() const;
  const std::set<Cell>& cells();
  const std::set<Cell>& initialPattern() const;

  void run();
  void clear();
  void reset();
  void pause();
  void finish();
  void speedUp();
  void slowDown();
  void increaseSize();
  void reduceSize();
  void generatePopulation(double density);
  void insertCell(const Cell& cell);
  void removeCell(const Cell& cell);
  void insertPattern(const std::set<Cell>& pattern);

 private:
  void update();

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
  std::set<Cell> m_cells;
  std::mutex m_mutex;
};

#endif
