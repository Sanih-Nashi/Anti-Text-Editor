#pragma once 

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <termio.h>

#define CTRL_KEY(x) ((x) & 0x1f)

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



extern struct utils::Terminal ter;
