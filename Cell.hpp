#ifndef GAME_OF_LIFE_CELL_HPP
#define GAME_OF_LIFE_CELL_HPP

struct Cell {
  int x{0};
  int y{0};

  bool operator==(const Cell &other) const {
    return x == other.x && y == other.y;
  }
  bool operator<(const Cell &other) const {
    if (y < other.y) {
      return true;
    }
    if (y > other.y) {
      return false;
    }
    if (x < other.x) {
      return true;
    }
    return false;
  }
  bool operator>(const Cell &other) const {
    return !(*this == other) && !(*this < other);
  }
};

#endif
