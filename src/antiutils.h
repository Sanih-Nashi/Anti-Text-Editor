#pragma once 

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <termio.h>

#include <vector>

#define CTRL_KEY(x) ((x) & 0x1f)
#define DEL_KEY 127

namespace utils{
  
  struct Terminal{
    int row;
    int column;
    int cx, cy; 
    struct termios orig_setting;

  };

  void die(const char* errormsg);
  void ClearScreen();
};

inline struct utils::Terminal ter;
