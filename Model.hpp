#ifndef GAME_OF_LIFE_MODEL_HPP
#define GAME_OF_LIFE_MODEL_HPP

#include <mutex>
#include <set>
#include <utility>

class Model {
 public:
  using Cell = std::pair<std::size_t, std::size_t>;

  enum class Status { Stopped, Paused, Running };

  Model(std::size_t width, std::size_t height);

  Status status() const;
  std::size_t speed() const;
  std::size_t width() const;
  std::size_t height() const;
  std::size_t generation() const;
  const std::set<Cell>& livingCells();
  const std::set<Cell>& deadCells();

  void run();
  void pause();
  void reset();
  void stop();

  void increaseSpeed();
  void decreaseSpeed();
  void addLivingCell(Cell coord);
  void removeLivingCell(Cell coord);
  void generateLivingCells(std::size_t count);

 private:
  void update();

  const std::size_t m_width;
  const std::size_t m_height;

  Status m_status;
  std::size_t m_speed;
  std::size_t m_generation;
  std::set<Cell> m_livingCells;
  std::set<Cell> m_deadCells;
  std::mutex m_mutex;
};

#endif
