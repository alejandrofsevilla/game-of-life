#ifndef GAME_OF_LIFE_RLE_HELPER_HPP
#define GAME_OF_LIFE_RLE_HELPER_HPP

#include <map>
#include <set>
#include <string>

#include "Cell.hpp"

namespace rle {
std::set<std::string> listPatternNames();
std::set<Cell> loadPattern(const std::string& fileName);
void savePattern(const std::string& name, const std::set<Cell> pattern);
}  // namespace rle

#endif
