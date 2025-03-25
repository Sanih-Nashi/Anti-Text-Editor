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

inline std::vector<std::string> lines;
inline std::vector<bool> used_col(Tl::GetTerminalColumn() ,false);
inline int current_col = 0;
inline int current_row = 0;
