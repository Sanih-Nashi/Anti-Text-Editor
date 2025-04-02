#include <errno.h>
#include <stdio.h>

#include <utility>
#include <iostream>
#include <string>

#include "keyProcess.h"
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
    char str[7];
    int len = snprintf(str, sizeof(str), "\033[%dC", current_row);
    write(STDOUT_FILENO, str, len);
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
	  int size = lines[current_col + 1].size();
	  if (size != 0)
	  {
	    char str[9];
	    int len = snprintf(str, sizeof(str), "\n\r\033[%dC", size); 
	    write(STDOUT_FILENO, str, len);
	  }
	  else
          {
            write(STDOUT_FILENO, "\n\r", 2);
	  }

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
        lines[current_col].erase(--current_row, 1);
        write(STDOUT_FILENO, "\r\033[K", 4);
        write(STDOUT_FILENO, lines[current_col].c_str(), lines[current_col].size());
  	    write(STDOUT_FILENO, "\r", 1);
	      if (current_row != 0){
          std::string row = "\033[" + std::to_string(current_row) + "C";
          write(STDOUT_FILENO, row.c_str(), row.size());
        } 	
      }

      else
      {  
        if (current_col != 0)
        {
          int size = lines[current_col - 1].size();
          std::string num = "\033[" + std::to_string(size) + "C";
          lines[current_col - 1] = lines[current_col - 1] + lines[current_col];
          lines.erase(lines.begin() + current_col--);
          current_row = size;

          write(STDOUT_FILENO, "\033[A", 3);
          write(STDOUT_FILENO, lines[current_col].c_str(), lines[current_col].size());

          for (int i = current_col + 1; i < lines.size(); i++){
            write(STDOUT_FILENO, "\n\r\033[K", 5);
            write(STDOUT_FILENO, lines[i].c_str(), lines[i].size());
          }      

          std::string num2 = "\033[" + std::to_string((lines.size() + 1) - current_col) + "A";
          write(STDOUT_FILENO, "\n\033\r[K", 5);
          write(STDOUT_FILENO, num2.c_str(), num2.size());
          write(STDOUT_FILENO, num.c_str(), num.size());


        }  
      }
      break;
    }

    case ENTER_KEY:
    {
      lines.insert(lines.begin() + current_col, lines[current_col].substr(0, current_row));
      lines[current_col + 1] = lines[current_col + 1].substr(current_row, lines[current_col + 1].size() - 1);

      write(STDOUT_FILENO, "\033[K", 3);
      for (int i = current_col + 1; i < lines.size(); i++)
      {
        write(STDOUT_FILENO, "\n\r\033[K", 5);
        write(STDOUT_FILENO, lines[i].c_str(), lines[i].size());
      }        
      std::string num = "\033[" + std::to_string(lines.size() - current_col -1) + "A";
      write(STDOUT_FILENO, "\n\r\033[K", 5);
      write(STDOUT_FILENO, num.c_str(), num.size());
      current_col++;
      current_row = 0;
      break;
    }
      
  }
}
