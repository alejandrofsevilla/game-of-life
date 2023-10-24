#include "RleHelper.hpp"

#include <filesystem>
#include <fstream>
#include <regex>

namespace {
constexpr auto f_patternsFolder{"../data/"};
constexpr auto f_nextRowSymbol{'$'};
constexpr auto f_deadCellSymbol{'b'};
constexpr auto f_survivingCellSymbol{'o'};
constexpr auto f_endOfPatternSymbol{'!'};
constexpr auto f_rleFileExtension{".rle"};
const std::regex f_rleCommentRegex{"#.*"};
const std::regex f_rleHeaderRegex{"x = [0-9]*, y = [0-9]*(, rule=.*)?"};
const std::regex f_rleContentRegex("([0-9]*)(o|b)(?:([0-9])*(\\$))?");

std::set<Cell> map(const std::string& pattern) {
  std::set<Cell> result;
  std::smatch match;
  Cell cell;
  auto it = pattern.cbegin();
  while (it != pattern.cend()) {
    if (std::regex_search(it, pattern.cend(), match, f_rleContentRegex)) {
      auto consecutiveCells{std::max(1, std::atoi(match[1].str().c_str()))};
      if (match[2] == f_survivingCellSymbol) {
        for (auto i = 0; i < consecutiveCells; i++) {
          result.insert(cell);
          cell.x++;
        }
      } else {
        cell.x += consecutiveCells;
      }
    }
    it = match.suffix().first;
    auto rowsSkipped{std::max(1, std::atoi(match[3].str().c_str()))};
    if (match[4] == f_nextRowSymbol) {
      cell.y += rowsSkipped;
      cell.x = 0;
    } else if (*it == f_endOfPatternSymbol) {
      it++;
    }
  }
  return result;
}

int calculatePatternWidth(const std::set<Cell>& pattern) {
  auto mostRightElement{
      std::max_element(pattern.cbegin(), pattern.cend(),
                       [](const auto& a, const auto& b) { return a.x < b.x; })};
  auto mostLeftElement{
      std::min_element(pattern.cbegin(), pattern.cend(),
                       [](const auto& a, const auto& b) { return a.x < b.x; })};
  return mostRightElement->x - mostLeftElement->x + 1;
}

int calculatePatternHeight(const std::set<Cell>& pattern) {
  auto mostTopElement{
      std::min_element(pattern.cbegin(), pattern.cend(),
                       [](const auto& a, const auto& b) { return a.y < b.y; })};
  auto mostBottomElement{
      std::max_element(pattern.cbegin(), pattern.cend(),
                       [](const auto& a, const auto& b) { return a.y < b.y; })};
  return mostBottomElement->y - mostTopElement->y + 1;
}
}  // namespace

namespace rle {
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

void savePattern(const std::string& name, const std::set<Cell> pattern) {
  std::ofstream ostrm;
  ostrm.open(f_patternsFolder + name + f_rleFileExtension);
  ostrm << "#N " << name << std::endl;
  ostrm << "x = " << calculatePatternWidth(pattern);
  ostrm << ", y = " << calculatePatternHeight(pattern) << std::endl;
  auto prevCol{-1};
  auto consecutiveCells{0};
  for (auto it = pattern.cbegin(); it != pattern.cend(); it++) {
    auto columnsSkipped{it->x - prevCol - 1};
    if (columnsSkipped > 0) {
      if (consecutiveCells > 0) {
        if (consecutiveCells > 1) {
          ostrm << consecutiveCells;
        }
        ostrm << f_survivingCellSymbol;
        prevCol++;
        consecutiveCells = 0;
      }
      if (columnsSkipped > 1) {
        ostrm << columnsSkipped << f_deadCellSymbol;
      } else if (columnsSkipped == 1) {
        ostrm << f_deadCellSymbol;
      }
    }
    consecutiveCells++;
    prevCol = it->x;
    auto next{it};
    std::advance(next, 1);
    auto rowsSkipped = next->y - it->y;
    if (next == pattern.end() || rowsSkipped > 0) {
      if (consecutiveCells > 0) {
        if (consecutiveCells > 0) {
          if (consecutiveCells > 1) {
            ostrm << consecutiveCells;
          }
          ostrm << f_survivingCellSymbol;
        }
      }
      if (rowsSkipped > 0) {
        if (rowsSkipped > 1) {
          ostrm << rowsSkipped;
        }
        ostrm << f_nextRowSymbol;
        consecutiveCells = 0;
        prevCol = -1;
      }
    }
  }
  ostrm << f_endOfPatternSymbol << std::endl;
  ostrm.close();
}
}  // namespace rle
