#include <errno.h>

#include <utility>
#include <iostream>
#include <string>

#include "KeyProcess.h"
#include "antiutils.h"
#include "terminal.h"


char KeyPress::ReadKey(){
  int read_num;
  char c;
  char buffer[32];
  snprintf(buffer, sizeof(buffer), "\x1b[%d;%dH", ter.cy + 1, ter.cx + 1);
  ter.cy++;
  ter.cx++;
  while ( (read_num = read(STDIN_FILENO, &c, 1)) != 1) 
  {
    if (read_num == -1 && errno != EAGAIN)
      utils::die("read");
  }
  return c;
}

void KeyPress::ProcessKeyPress(){
  char c = ReadKey();

  if (31 < c && c < 127)
  {
    current_row++;
    used_col[current_col] = true;
    lines[current_col].insert(current_row - 1, std::string(1, c));
    write(STDOUT_FILENO, "\r", 1);
    write(STDOUT_FILENO, lines[current_col].c_str(), lines[current_col].size());
    write(STDOUT_FILENO, "\r", 1);
    std::string str = "\033[" + std::to_string(current_row) + "C";
    write(STDOUT_FILENO, str.c_str(), str.size());
  }

  switch(c){
    case CTRL_KEY('q'): 
    {
      
      write(STDOUT_FILENO, "\x1b[2J", 4);
      write(STDOUT_FILENO, "\x1b[H", 3);

      exit(0);
    }
    case CTRL_KEY('s'): 
    {
      utils::CommitChanges();
      break;
    }

    case CTRL_KEY('k'): 
    {
      if (current_col > 0)
      {
        if (current_row <= lines[current_col - 1].size())
          write(STDOUT_FILENO, "\033[1A", 4);

        else
        {
          write(STDOUT_FILENO, "\033[1A\r", 5);
          current_row = 0;
        }

        current_col--;
      }
      break;
    }
    case CTRL_KEY('j'): 
    {

      if (current_col < Tl::GetTerminalColumn() && current_col < lines.size() - 1)
      {
        
        if (current_row <= lines[current_col + 1].size()) 
          write(STDOUT_FILENO, "\n", 1);

        else
        {
          write(STDOUT_FILENO, "\n\r", 2);
	  std::string num = "\033[" + std::to_string(lines[current_col + 1].size()) + "C";
          write(STDOUT_FILENO, num.c_str(), num.size());
          current_row = lines[current_col + 1].size();
        }

        current_col++;
      }
      break;
    }
    case CTRL_KEY('h'): 
    {
      if (current_row > 0)
      {
        write(STDOUT_FILENO, "\b", 1);
        current_row--;
      }
      break;
    }
    case CTRL_KEY('l'): 
    {
      if (current_row < lines[current_col].size())
      {
        write(STDOUT_FILENO, "\033[C", 3);
	      current_row++;
      }
      break;
    }

    case DEL_KEY: 
    { 
      if (current_row != 0)
      {
        lines[current_col].erase(current_row - 1, 1);
        write(STDOUT_FILENO, "\r", 1);
        write(STDOUT_FILENO, "\033[K", 3);
        write(STDOUT_FILENO, lines[current_col].c_str(), lines[current_col].size());
        write(STDOUT_FILENO, "\r", 1);
        std::string row = "\033[" + std::to_string(--current_row) + "C";
        write(STDOUT_FILENO, row.c_str(), row.size());
      }
      else
      {  
        if (current_col != 0)
	{
	  write(STDOUT_FILENO, "\033[A", 3);
	  std::string num = "\033[" + std::to_string(lines[current_col - 1].size()) + "C";
          write(STDOUT_FILENO, num.c_str(), num.size());
          current_row = lines[current_col - 1].size();
	  current_col--;
	}
      }  
      break;
    }

    case ENTER_KEY:
    {
      write(STDOUT_FILENO, "\n\r", 2);
      lines.insert(lines.begin() + current_col, &c);
      current_col++;
      break;
    }
      
  }
}
