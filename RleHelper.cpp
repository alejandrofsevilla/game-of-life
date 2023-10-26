#include "RleHelper.hpp"

#include <filesystem>
#include <fstream>
#include <regex>

namespace {
constexpr auto f_patternsFolder{"../patterns/"};
constexpr auto f_nextRowSymbol{'$'};
constexpr auto f_deadCellSymbol{'b'};
constexpr auto f_aliveCellSymbol{'o'};
constexpr auto f_endOfLine{'\n'};
constexpr auto f_endOfPatternSymbol{'!'};
constexpr auto f_rleFileExtension{".rle"};
const std::regex f_rleCommentRegex{"#.*"};
const std::regex f_rleHeaderRegex{"x = [0-9]*, y = [0-9]*(, rule=.*)?"};
const std::regex f_rleContentRegex("([0-9]*)(o|b)(?:([0-9]*)(\\$))?");

std::set<Cell> map(const std::string &pattern) {
  std::set<Cell> result;
  std::smatch match;
  Cell cell;
  auto it = pattern.cbegin();
  while (it != pattern.cend()) {
    if (std::regex_search(it, pattern.cend(), match, f_rleContentRegex)) {
      auto consecutiveCells{std::max(1, std::atoi(match[1].str().c_str()))};
      if (match[2] == f_aliveCellSymbol) {
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

Cell mostLeftCell(const std::set<Cell> &pattern) {
  return *std::min_element(
      pattern.cbegin(), pattern.cend(),
      [](const auto &a, const auto &b) { return a.x < b.x; });
}

Cell mostRightCell(const std::set<Cell> &pattern) {
  return *std::max_element(
      pattern.cbegin(), pattern.cend(),
      [](const auto &a, const auto &b) { return a.x < b.x; });
}

Cell mostTopCell(const std::set<Cell> &pattern) {
  return *std::min_element(
      pattern.cbegin(), pattern.cend(),
      [](const auto &a, const auto &b) { return a.y < b.y; });
}

Cell mostBottomCell(const std::set<Cell> &pattern) {
  return *std::max_element(
      pattern.cbegin(), pattern.cend(),
      [](const auto &a, const auto &b) { return a.y < b.y; });
}
}  // namespace

namespace rle {
std::set<std::string> listPatternNames() {
  std::set<std::string> files;
  if (!std::filesystem::exists(f_patternsFolder)) {
    return {};
  }
  for (const auto &file :
       std::filesystem::directory_iterator(f_patternsFolder)) {
    if (file.path().extension().string() == f_rleFileExtension) {
      files.insert(file.path().filename().stem().string());
    }
  }
  return files;
}

std::set<Cell> loadPattern(const std::string &name) {
  std::string pattern;
  std::string line;
  std::ifstream istrm;
  istrm.open(f_patternsFolder + name + f_rleFileExtension);
  while (std::getline(istrm, line)) {
    if (!std::regex_search(line, f_rleCommentRegex) &&
        !std::regex_search(line, f_rleHeaderRegex)) {
      if (line.back() == f_endOfLine) {
        line.pop_back();
      }
      pattern.append(line);
    }
  }
  istrm.close();
  return map(pattern);
}

void savePattern(const std::string &name, const std::set<Cell> pattern) {
  if (pattern.empty()) {
    return;
  }
  if (!std::filesystem::is_directory(f_patternsFolder) ||
      !std::filesystem::exists(f_patternsFolder)) {
    std::filesystem::create_directory(f_patternsFolder);
  }
  std::ofstream ostrm;
  ostrm.open(f_patternsFolder + name + f_rleFileExtension);
  ostrm << "#N " << name << std::endl;
  auto minCol{mostLeftCell(pattern).x};
  auto minRow{mostTopCell(pattern).y};
  auto maxCol{mostRightCell(pattern).x};
  auto maxRow{mostBottomCell(pattern).y};
  ostrm << "x = " << maxCol - minCol;
  ostrm << ", y = " << maxRow - minRow << std::endl;
  auto prevCol{0};
  auto prevRow{0};
  auto consecutiveCells{0};
  for (auto it = pattern.cbegin(); it != pattern.cend(); it++) {
    auto col = it->x - minCol;
    auto row = it->y - minRow;
    if (it == pattern.cbegin()) {
      prevCol = col - 1;
      prevRow = row - 1;
    }
    auto columnsSkipped{col - prevCol};
    auto rowsSkipped{row - prevRow};
    auto next{it};
    std::advance(next, 1);
    if (rowsSkipped > 0 || columnsSkipped > 1) {
      if (consecutiveCells > 0) {
        if (consecutiveCells > 1) {
          ostrm << consecutiveCells;
        }
        consecutiveCells = 0;
        ostrm << f_aliveCellSymbol;
      }
      if (rowsSkipped > 0) {
        if (it != pattern.cbegin()) {
          if (rowsSkipped > 1) {
            ostrm << rowsSkipped;
          }
          ostrm << f_nextRowSymbol;
        }
        columnsSkipped = col + 1;
      }
      if (columnsSkipped > 1) {
        if (columnsSkipped > 2) {
          ostrm << columnsSkipped - 1;
        }
        ostrm << f_deadCellSymbol;
      }
    }
    consecutiveCells++;
    prevRow = row;
    prevCol = col;
  }
  if (consecutiveCells > 0) {
    if (consecutiveCells > 1) {
      ostrm << consecutiveCells;
    }
    consecutiveCells = 0;
    ostrm << f_aliveCellSymbol;
  }
  ostrm << f_endOfPatternSymbol << std::endl;
  ostrm.close();
}
}  // namespace rle
