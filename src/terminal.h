#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <utility>
#include <cstdint>


namespace Tl{
  int GetTerminalRow();
  int GetTerminalColumn();
  int GetTerminalSize(int& row, int& column);
}
void initTerminal();

inline std::unordered_map <int, std::string> changed_lines;
inline std::vector<std::pair<size_t, std::pair<size_t, uint8_t>>> changes;
inline std::vector<bool> used_col(Tl::GetTerminalColumn() ,false);
inline int current_col = 1;
inline int current_row = 1;
