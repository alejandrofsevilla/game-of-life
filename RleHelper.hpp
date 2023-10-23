#ifndef GAME_OF_LIFE_RLE_HELPER_HPP
#define GAME_OF_LIFE_RLE_HELPER_HPP

#include <map>
#include <set>
#include <string>

#include "Model.hpp"

namespace rle {

std::set<std::string> listPatternNames();
std::set<Model::Cell> patternFromName(const std::string& fileName);

}  // namespace rle

#endif
