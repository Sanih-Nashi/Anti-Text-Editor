#pragma once 

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <termio.h>

#include <vector>
#include <string>

#define CTRL_KEY(x) ((x) & 0x1f)
#define DEL_KEY 127
#define ENTER_KEY 13

#define DEV_DEBUG

namespace utils{
  
  struct Terminal{
    int row;
    int column;
    int cx, cy; 
    struct termios orig_setting;

  };
  
  std::string GetSpecificLine(const int& target_line, std::ifstream& file);
  char CommitChanges();
  void die(const char* errormsg);
  void ClearScreen();
};

inline struct utils::Terminal ter;
#define USABLE_TER_ROW (ter.row - 2)
