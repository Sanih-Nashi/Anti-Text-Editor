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
void initTerminal(char ***argv, const int& argc);

inline std::vector<std::string> lines;
inline bool is_file_mentioned;
inline std::string file_name;
inline int current_col = 0;
inline int current_row = 0;
