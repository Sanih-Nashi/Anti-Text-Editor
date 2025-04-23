#include "terminal.h"
#include "antiutils.h"

#include <sys/ioctl.h>
#include <unistd.h>

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

void AlignTer()
{
  if (Tl::GetTerminalSize(ter.row, ter.column) == -1)
    utils::die("cannot get terminal size");

  write(STDOUT_FILENO, "\033[2J\033[H", 7);
  for (int i = current_line - current_row; i < current_line + USABLE_TER_ROW - current_row 
      && i < lines.size() ; i++)
  {
    write(STDOUT_FILENO, lines[i].c_str(), lines[i].size());
    write(STDOUT_FILENO, "\n\r", 2);
  }
  write(STDOUT_FILENO, "\033[H", 3);
  char str[19];
  int len;
  
  if (current_row == 0)
    len = snprintf(str, sizeof(str), "\033[H\033[%dC", current_col);
  else if (current_col == 0)
    len = snprintf(str, sizeof(str), "\033[H\033[%dB", current_row);
  else if (current_row == 0 && current_col == 0)
  {
    len = snprintf(str, sizeof(str), "\033[H");
  }
  else
    len = snprintf(str, sizeof(str), "\033[H\033[%dB\033[%dC", current_row, current_col);
  
  write(STDOUT_FILENO, str, len);
 
}

void initTerminal(char*** argv, const int& argc) {
    
  if (Tl::GetTerminalSize(ter.row, ter.column) == -1) 
    utils::die("getWindowSize");
  
  if (argc < 2)
  {
    is_file_mentioned = false;
    lines.push_back(std::string(""));
    return;
  }

  is_file_mentioned = true;
  file_name = (*argv)[1];
  std::ifstream file_in(file_name);

  if (!file_in.is_open())
  {
    std::cout <<"error opening file";
    exit(1);
  }
  write(STDOUT_FILENO, "\033[H", 3);
  std::string buffer;
  for (int i = 0; std::getline(file_in, buffer); i++)
  {
    lines.push_back(buffer);
    if (i < USABLE_TER_ROW)
    {
      write(STDOUT_FILENO, buffer.c_str(), buffer.size());
      write(STDOUT_FILENO, "\n\r", 2);
    }
  }
  write(STDOUT_FILENO, "\033[H", 3);

}
