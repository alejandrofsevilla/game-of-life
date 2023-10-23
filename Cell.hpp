#ifndef GAME_OF_LIFE_CELL_HPP
#define GAME_OF_LIFE_CELL_HPP

struct Cell {
  enum class Status { Dead, Alive };

  int x{0};
  int y{0};
  Status status{Status::Alive};

  bool operator==(const Cell& other) const {
    return x == other.x && y == other.y;
  }
  bool operator<(const Cell& other) const {
    if (x < other.x) {
      return true;
    }
    if (x > other.x) {
      return false;
    }
    if (y < other.y) {
      return true;
    }
    return false;
  }
  bool operator>(const Cell& other) const {
    return !(*this == other) && !(*this < other);
  }
};

#endif
