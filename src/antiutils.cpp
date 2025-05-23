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
  write(STDOUT_FILENO, "\033[32m", 5);
}

inline void SetTerminaltoDefaultColor()
{
  write(STDOUT_FILENO, "\033[0m", 4);
}

void utils::GoToMainTypingArea(const int& size)
{
 
  std::string rp_string(18 + size, ' ');
  SetTerminaltoDefaultColor();
  write(STDOUT_FILENO, "\r", 1);
  write(STDOUT_FILENO, rp_string.c_str(), 18 + size);
  write(STDOUT_FILENO, "\r\033[A", 4);
  write(STDOUT_FILENO, rp_string.c_str(), 18);
  write(STDOUT_FILENO, "\033[H", 3);

  char str2[16];
  int len2 = snprintf(str2, sizeof(str2), "\033[%d;%dH", current_row, current_col + 1);
  write(STDOUT_FILENO, str2, len2);
}

char utils::CommitChanges()
{
  std::ofstream file_out;

  if (!is_file_mentioned)
  {
    char str[13];
    int len = snprintf(str, sizeof(str), "\033[%d;1H\033[A", ter.column);
    write(STDOUT_FILENO, str, len);
    write(STDOUT_FILENO, "\033[31m", 5);
    write(STDOUT_FILENO, "Ctrl + C to cancel", 18);
    SetTerminaltoDefaultColor();
    write(STDOUT_FILENO, "\n\r", 2);
    write(STDOUT_FILENO, "\033[31m", 5);
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

      else if (c == DEL_KEY && file.size() > 0)
      {
        file.pop_back();
        SetTerminaltoDefaultColor();
	      write(STDOUT_FILENO, "\b \b", 3);
	      ColorTerminalUser();
      }

      else if(c == CTRL_KEY('c'))
      {  
        GoToMainTypingArea(file.size());
	      return -1;
      }

      c = KeyPress::ReadKey();
    }
    file_name = file;
    file_out = std::ofstream(file);
    GoToMainTypingArea(file.size());   
    is_file_mentioned = true;
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

  char str[10];
  int len = snprintf(str, sizeof(str), "\033[%d;1H", ter.column);
  write(STDOUT_FILENO, str, len);
  write(STDOUT_FILENO, "\033[32m", 5); //sets color to green
  
  char str2[100];
  int len2 = snprintf(str2, sizeof(str2), "Saved File in %s", file_name.c_str());
  write(STDOUT_FILENO, str2, len2);
  SetTerminaltoDefaultColor();
  
  char str3[19];
  int len3;
  
  if (current_row == 0)
    len3 = snprintf(str3, sizeof(str3), "\033[H\033[%dC", current_col);
  else if (current_col == 0)
    len3 = snprintf(str3, sizeof(str3), "\033[H\033[%dB", current_row);
  else if (current_row == 0 && current_col == 0)
  {
    len3 = snprintf(str3, sizeof(str3), "\033[H");
  }
  else
    len3 = snprintf(str3, sizeof(str3), "\033[H\033[%dB\033[%dC", current_row, current_col);
  
  write(STDOUT_FILENO, str3, len3);

  char c = KeyPress::ReadKey();
   
  char str4[10];
  int len4 = snprintf(str4, sizeof(str4), "\033[%d;1H", ter.column);
  write(STDOUT_FILENO, str4, len4);
  // std::string cpp_str(len2, ' ');
  write(STDOUT_FILENO, std::string(len2, ' ').c_str(), len2);
  write(STDOUT_FILENO, str3, len3);

  return c;
}
