#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <utility>

namespace Tl{
  int GetTerminalRow();
  int GetTerminalColumn();
  int GetTerminalSize(int& row, int& column);
}
void initTerminal();

inline std::unordered_map<size_t, std::string> changed_line;
inline std::unordered_map<size_t, std::pair<size_t, u_int8_t>> changes;
inline std::vector<bool> used_col(Tl::GetTerminalColumn() ,false);
inline int current_col = 0;
inline int current_row = 0;