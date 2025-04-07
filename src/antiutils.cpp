#include "antiutils.h"
#include "terminal.h"
#include "keyProcess.h"

#include <string>
#include <iostream>
#include <fstream>

void utils::die(const char *errormsg)
{

  write(STDOUT_FILENO, "\x1b[2J", 4);
  write(STDOUT_FILENO, "\x1b[H", 3);

  perror(errormsg);
  exit(1);
}

void utils::ClearScreen()
{
  std::cout <<ter.column <<"size\n";
  write(STDOUT_FILENO, "\x1b[2J", 4); // 1b is hexadecimal for 27 and '[' which is a start of an escape sequence
  // J means to erase and '2' is the argument which means clear the entire screen}
  write(STDOUT_FILENO, "\x1b[H", 3); // H for moving the cursor, here to the beginning
}

inline void ColorTerminalUser(){
  write(STDOUT_FILENO, "\033[47m\033[30m", 10);
}

inline void SetTerminaltoDefaultColor()
{
  write(STDOUT_FILENO, "\033[0m", 4);
}

inline void GoToMainTypingArea(const int& size)
{
 
  std::string rp_string(18 + size, ' ');
  SetTerminaltoDefaultColor();
  write(STDOUT_FILENO, "\r", 1);
  write(STDOUT_FILENO, rp_string.c_str(), 18 + size);
  write(STDOUT_FILENO, "\r\033[A", 4);
  write(STDOUT_FILENO, rp_string.c_str(), 18);
  write(STDOUT_FILENO, "\033[H", 3);

  char str2[16];
  int len2 = snprintf(str2, sizeof(str2), "\033[%d;%dH", current_col - 1, current_row + 1);
  write(STDOUT_FILENO, str2, len2);
}

void utils::CommitChanges()
{
  std::ofstream file_out;

  if (!is_file_mentioned)
  {
    char str[9];
    int len = snprintf(str, sizeof(str), "\033[%d;1H", ter.column);
    write(STDOUT_FILENO, str, len);
    write(STDOUT_FILENO, "\033[41m", 5);
    //write(STDOUT_FILENO, "Ctrl + C to escape", 18);
    //SetTerminaltoDefaultColor();
    //write(STDOUT_FILENO, "\n\r\033[41m", 7);
    write(STDOUT_FILENO, "Name of the File: ", 18);
    ColorTerminalUser();

    char c = KeyPress::ReadKey();
    std::string file = "";

    while (c != ENTER_KEY)
    {
      if (31 < c && c < 127)
      {
        file.push_back(c);
	std::string str = std::string(1, c);
	write(STDOUT_FILENO, str.c_str(), str.size());
      }

      else if (c == DEL_KEY)
      {
        file.pop_back();
        SetTerminaltoDefaultColor();
	write(STDOUT_FILENO, "\b \b", 3);
	ColorTerminalUser();
      }

      else if(c == CTRL_KEY('c'))
      {  
        GoToMainTypingArea(file.size());
	return;
      }

      c = KeyPress::ReadKey();
    }

    file_out = std::ofstream(file);
    GoToMainTypingArea(file.size());   

  }
  
  else
  {
    file_out = std::ofstream(file_name);
    SetTerminaltoDefaultColor();
  }

  for (std::string& ref : lines)
  {
    file_out <<ref <<"\n";
  }	  
}
