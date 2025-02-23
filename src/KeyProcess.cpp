#include <errno.h>

#include <utility>
#include <iostream>

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
    used_col[current_col] = true;
    write(STDOUT_FILENO, &c, 1);
    changes[current_col] = std::make_pair(current_row, c);
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
      std::cout <<current_col <<", " <<current_row;
      utils::CommitChanges();
      break;
    }

    case CTRL_KEY('i'): 
    {
      if (current_col > 0)
      {
        if (used_col[current_col - 1])
          write(STDOUT_FILENO, "\033[1A", 4);

        else
          write(STDOUT_FILENO, "\033[1A\r", 5);

        current_col--;
      }
      break;
    }
    case CTRL_KEY('k'): 
    {

      if (current_col <= Tl::GetTerminalColumn())
      {
        
        if (used_col[current_col + 1])
          write(STDOUT_FILENO, "\n", 1);

        else
          write(STDOUT_FILENO, "\n\r", 2);

        current_col++;
      }
      break;
    }
    case CTRL_KEY('j'): 
    {
      write(STDOUT_FILENO, "\b", 1);
      current_row--;
      break;
    }
    case CTRL_KEY('l'): 
    {
      write(STDOUT_FILENO, "\033[C", 3);
      current_row++;
      break;
    }

    case DEL_KEY: 
    {
      write(STDOUT_FILENO, "\b \b", 3);
      current_row--;
      break;
    }

  }
}
