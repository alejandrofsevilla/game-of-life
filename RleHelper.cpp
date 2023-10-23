#include "RleHelper.hpp"

#include <filesystem>
#include <fstream>
#include <regex>

namespace {
constexpr auto f_patternsFolder{"../data/"};
constexpr auto f_nextRowSymbol{'$'};
constexpr auto f_survivingCellSymbol{'o'};
constexpr auto f_endOfPatternSymbol{'!'};
constexpr auto f_rleFileExtension{".rle"};
const std::regex f_rleCommentRegex{"#.*"};
const std::regex f_rleHeaderRegex{"x = [0-9]*, y = [0-9]*(, rule=.*)?"};
const std::regex f_rleContentRegex("([0-9]*)(o|b)");

std::set<Cell> map(const std::string& pattern) {
  std::set<Cell> result;
  std::smatch match;
  Cell cell;
  auto it = pattern.cbegin();
  while (it != pattern.cend()) {
    if (std::regex_search(it, pattern.cend(), match, f_rleContentRegex)) {
      auto repetitions{std::max(1, std::atoi(match[1].str().c_str()))};
      if (match[2] == f_survivingCellSymbol) {
        for (auto i = 0; i < repetitions; i++) {
          result.insert(cell);
          cell.x++;
        }
      } else {
        cell.x += repetitions;
      }
    }
    it = match.suffix().first;
    if (*it == f_nextRowSymbol) {
      cell.y++;
      cell.x = 0;
      it++;
    } else if (*it == f_endOfPatternSymbol) {
      it++;
    }
  }
  return result;
}
}  // namespace

namespace rle {
std::set<Cell> loadPattern(const std::string& fileName) {
  std::string pattern;
  std::string line;
  std::ifstream istrm;
  istrm.open(f_patternsFolder + fileName + f_rleFileExtension);
  while (std::getline(istrm, line)) {
    if (!std::regex_search(line, f_rleCommentRegex) &&
        !std::regex_search(line, f_rleHeaderRegex)) {
      if (line.back() != f_endOfPatternSymbol) {
        line.pop_back();
      }
      pattern.append(line);
    }
  }
  istrm.close();
  return map(pattern);
}

std::set<std::string> listPatternNames() {
  std::set<std::string> files;
  for (const auto& file :
       std::filesystem::directory_iterator(f_patternsFolder)) {
    if (file.path().extension().string() == f_rleFileExtension) {
      files.insert(file.path().filename().stem().string());
    }
  }
  return files;
}
}  // namespace rle
