#ifndef GAME_OF_LIFE_MODEL_HPP
#define GAME_OF_LIFE_MODEL_HPP

#include <mutex>
#include <set>
#include <utility>

class Model {
 public:
  using Cell = std::pair<std::size_t, std::size_t>;

  enum class Status { Uninitialized, Stopped, Running, Paused, Finished };

  Model(std::size_t maxWidth, std::size_t maxHeight);

  Status status() const;
  std::size_t speed() const;
  std::size_t width() const;
  std::size_t height() const;
  std::size_t generation() const;
  const std::set<Cell>& livingCells();
  const std::set<Cell>& deadCells();

  void run();
  void clear();
  void reset();
  void pause();
  void finish();
  void speedUp();
  void slowDown();
  void increaseSize();
  void reduceSize();
  void addLivingCell(Cell coord);
  void removeLivingCell(Cell coord);
  void generatePopulation(double populationRate);

 private:
  void update();

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
  std::set<Cell> m_pattern;
  std::set<Cell> m_livingCells;
  std::set<Cell> m_deadCells;
  std::mutex m_mutex;
};

#endif
