#include "terminal.h"
#include "antiutils.h"

#include "sys/ioctl.h"
#include "unistd.h"

#include <iostream>
#include <string>
#include <fstream>

int Tl::GetTerminalRow(){
  struct winsize window_size;

  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &window_size) == -1 || window_size.ws_col == 0)
    return -1;
    
  else
    return window_size.ws_row;
}

int Tl::GetTerminalColumn(){
  struct winsize window_size;

  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &window_size) == -1 || window_size.ws_col == 0)
    return -1;

  else
    return window_size.ws_col;
}

int Tl::GetTerminalSize(int& row, int& column){
  struct winsize window_size;

  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &window_size) == -1 || window_size.ws_col == 0)
    return -1;
  else{
    column = window_size.ws_col;
    row = window_size.ws_row;
    return 0;
  }
}


void initTerminal() {
  used_col[0] = true;
  
  if (Tl::GetTerminalSize(ter.row, ter.column) == -1) 
    utils::die("getWindowSize");
  ter.cx = 0;
  ter.cy = 0;
  
  std::ifstream file_in("test.txt");
  if (!file_in.is_open())
  {
    std::cout <<"error opening file";
    exit(1);
  }
  std::string buffer;
  for (int i = 0; std::getline(file_in, buffer); i++)
  {
    lines.push_back(buffer);
    if (i < Tl::GetTerminalRow())
    {
      write(STDOUT_FILENO, buffer.c_str(), buffer.size());
      write(STDOUT_FILENO, "\n\r", 2);
    }
  }
  write(STDOUT_FILENO, "\033[H", 3);
}
