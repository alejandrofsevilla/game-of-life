#ifndef GAME_OF_LIFE_CELL_HPP
#define GAME_OF_LIFE_CELL_HPP

struct Cell {
  enum class Status { Empty, Alive, Dead };

  std::size_t col{0};
  std::size_t row{0};

  bool operator==(const Cell &other) const {
    return col == other.col && row == other.row;
  }
  bool operator<(const Cell &other) const {
    if (row < other.row) {
      return true;
    }
    if (row > other.row) {
      return false;
    }
    if (col < other.col) {
      return true;
    }
    return false;
  }
  bool operator>(const Cell &other) const {
    return !(*this == other) && !(*this < other);
  }
};

#endif
