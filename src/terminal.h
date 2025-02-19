#pragma once

#include <vector>

namespace Tl{
  int GetTerminalRow();
  int GetTerminalColumn();
  int GetTerminalSize(int& row, int& column);
}
void initTerminal();

inline std::vector<bool> used_col(Tl::GetTerminalColumn() ,false);
inline int current_col = 0;
inline int current_row = 0;