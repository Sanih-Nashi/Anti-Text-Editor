#include <errno.h>
#include <stdio.h>

#include <utility>
#include <iostream>
#include <string>

#include "keyProcess.h"
#include "antiutils.h"
#include "terminal.h"


inline void StateLine()
{
  char str[40];
  int len = snprintf(str, sizeof(str), "\033[%d;1H", ter.column + 1);    // goes to the bottom left
  write(STDOUT_FILENO, str, len);

  write(STDOUT_FILENO, "\033[K", 3); //cleares the text printed before

  len = snprintf(str, sizeof(str), "\033[47m\033[30m%d, %d\033[0m", current_line + 1, current_col);
  //changes the bg to white and fg to black and resets after printing the text by \033[0m esc sequence
  write(STDOUT_FILENO, str, len);


  // positioning the cursor
  if (current_row == 0 && current_col != 0)
    len = snprintf(str, sizeof(str), "\033[H\033[%dC", current_col);
  else if (current_row != 0 && current_col == 0)
    len = snprintf(str, sizeof(str), "\033[H\033[%dB", current_row);
  else if (current_row == 0 && current_col == 0)
    len = snprintf(str, sizeof(str), "\033[H");
  else
    len = snprintf(str, sizeof(str), "\033[H\033[%dB\033[%dC", current_row, current_col);

  write(STDOUT_FILENO, str, len);

}

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
  StateLine();
  char c = ReadKey();

RepeatKeyProcessing:

  if (31 < c && c < 127)
  {
    current_col++;
    lines[current_line].insert(current_col - 1, std::string(1, c));
    write(STDOUT_FILENO, "\r", 1);
    write(STDOUT_FILENO, lines[current_line].c_str(), lines[current_line].size());
    write(STDOUT_FILENO, "\r", 1);
    char str[7];
    int len = snprintf(str, sizeof(str), "\033[%dC", current_col);
    write(STDOUT_FILENO, str, len);
  }

  switch(c){
    case CTRL_KEY('q'):
    {

      write(STDOUT_FILENO, "\x1b[2J", 4);
      write(STDOUT_FILENO, "\x1b[H", 3);

      exit(0);
    }
#ifdef DEV_DEBUG
    case CTRL_KEY('d'):
    {
      char str[40];
      int len = snprintf(str, sizeof(str), "row: %d, col: %d, max_col: %d", current_row, current_col, USABLE_TER_ROW);
      write(STDOUT_FILENO, str, len);
      break;
    } 
#endif
    case CTRL_KEY('s'):
    {
      c = utils::CommitChanges();
      if (c != -1)
        goto RepeatKeyProcessing;
      break;
    }

    case CTRL_KEY('k'): 
    {
      if (current_row > 0)
      {
        if (current_col <= lines[current_line - 1].size())
          write(STDOUT_FILENO, "\033[A", 3);

        else
        {
	        char str[11];
	        int size = lines[current_line - 1].size();
	        int len = snprintf(str, sizeof(str), "\033[A\r\033[%dC", size);
          write(STDOUT_FILENO, str, len);
          current_col = size;
        }

        current_row--;
        current_line--;
      }
      
      else if (current_row == 0 && current_line > 0)
      {
	      write(STDOUT_FILENO, "\033[H", 3);
        write(STDOUT_FILENO, "\033[K", 3);
        for (int i = --current_line; i < current_line + USABLE_TER_ROW; i++)
	      {
          write(STDOUT_FILENO, lines[i].c_str(), lines[i].size());
          write(STDOUT_FILENO, "\n\r\033[K", 5);
	      }
	      write(STDOUT_FILENO, "\033[H", 3);
	      if (current_col != 0)
	      {
	        char str[22];
	        int len = snprintf(str, sizeof(str), "\033[%dC", current_col);
	      }
      }
	  	
      break;
    }
    case CTRL_KEY('j'): 
    {

      if (current_row < USABLE_TER_ROW - 1 && current_line < lines.size() - 1)
      {
        
        if (current_col <= lines[current_line + 1].size()) 
          write(STDOUT_FILENO, "\n", 1);

        else
        {
	       int size = lines[current_line + 1].size();
	       if (size != 0)
	       {
	        char str[9];
	        int len = snprintf(str, sizeof(str), "\n\r\033[%dC", size); 
	        write(STDOUT_FILENO, str, len);
	       }
	        else
            write(STDOUT_FILENO, "\n\r", 2);

	        current_col = lines[current_line + 1].size();
        }
        current_row++;
        current_line++;
      }

      else if (current_row == USABLE_TER_ROW - 1 && current_line < lines.size() -1)
      {
    	  write(STDOUT_FILENO, "\033[H", 3);
        write(STDOUT_FILENO, "\033[K", 3);
	      for (int i = ++current_line - USABLE_TER_ROW + 1; i <= current_line; i++)
	      {
	        write(STDOUT_FILENO, lines[i].c_str(), lines[i].size()); 
          write(STDOUT_FILENO, "\n\r\033[K", 5);
	      }
        write(STDOUT_FILENO, "\033[A\r", 4);
        
        if(current_col <= lines[current_line].size()){
          if (current_col != 0)
          {
            char str[22];
            int len = snprintf(str, sizeof(str), "\033[%dC", current_col);
            write(STDOUT_FILENO, str, len);
          }
          else
            write(STDOUT_FILENO, "\r", 1);

        }

        else
        {
          if (lines[current_line].size() != 0)
          {
            char str[22];
            int len = snprintf(str, sizeof(str), "\033[%dC", lines[current_col].size());
            write(STDOUT_FILENO, str, len);
          }
          else
            write(STDOUT_FILENO, "\r", 1);

          current_col = lines[current_col].size();
        }

        }

      break;
    }
    case CTRL_KEY('h'): 
    {
      if (current_col > 0)
      {
        write(STDOUT_FILENO, "\b", 1);
        current_col--;
      }
      break;
    }
    case CTRL_KEY('l'): 
    {
      if (current_col < lines[current_line].size())
      {
        write(STDOUT_FILENO, "\033[C", 3);
	      current_col++;
      }
      break;
    }

    case DEL_KEY: 
    { 
      if (current_col != 0)
      {
        lines[current_line].erase(--current_col, 1);
        write(STDOUT_FILENO, "\r\033[K", 4);
        write(STDOUT_FILENO, lines[current_line].c_str(), lines[current_line].size());
  	    write(STDOUT_FILENO, "\r", 1);
        if (current_col != 0){
          // std::string row = "\033[" + std::to_string(current_col) + "C";
          char str[7];
          int len = snprintf(str, sizeof(str), "\033[%dC", current_col); 
          write(STDOUT_FILENO, str, len);
        }
      }

      else
      {
        if (current_row != 0)
        {
          int size = lines[current_line - 1].size();
          // std::string num = "\033[" + std::to_string(size) + "C";

	        lines[current_line - 1] = lines[current_line - 1] + lines[current_line];
          lines.erase(lines.begin() + current_line--);
	        current_row--;
          current_col = size;

          write(STDOUT_FILENO, "\033[A", 3);
          write(STDOUT_FILENO, lines[current_line].c_str(), lines[current_line].size());

          for (int i = current_line + 1;
               i < (current_line - current_row) + USABLE_TER_ROW && i < lines.size();
              i++)
            {
            write(STDOUT_FILENO, "\n\r\033[K", 5);
            write(STDOUT_FILENO, lines[i].c_str(), lines[i].size());
          }

          write(STDOUT_FILENO, "\n\r\033[K\033[H", 8);
	        if (current_row != 0 )
	        {
            char str2[7];
            int len2 = snprintf(str2, sizeof(str2), "\033[%dB", current_row);
	          write(STDOUT_FILENO, str2, len2);
	         }
	        else if (current_col != 0)
	        {
	          char str[7];
            int len = snprintf(str, sizeof(str), "\033[%dC", size);
            write(STDOUT_FILENO, str, len);
	        }

        }
      }
      break;
    }

    case ENTER_KEY:
    {
      lines.insert(lines.begin() + current_line, lines[current_line].substr(0, current_col));
      lines[current_line + 1] = lines[current_line + 1].substr(current_col, lines[current_line + 1].size());
      if (current_row < USABLE_TER_ROW - 1)
      {
        write(STDOUT_FILENO, "\033[K", 3);
        for (int i = current_line + 1; i < (current_line + 1) + (USABLE_TER_ROW - (current_row + 1)) && i < lines.size(); i++)
        {
          write(STDOUT_FILENO, "\n\r\033[K", 5);
          write(STDOUT_FILENO, lines[i].c_str(), lines[i].size());
        }
        write(STDOUT_FILENO, "\033[H", 3);

        char str[22];
        int len = snprintf(str, sizeof(str), "\033[%dB\r", ++current_row);
        //write(STDOUT_FILENO, "\n\r\033[K", 5);
        write(STDOUT_FILENO, str, len);
      }
      else
      {
        write(STDOUT_FILENO, "\033[H", 3);
        write(STDOUT_FILENO, lines[current_line - USABLE_TER_ROW + 1].c_str()
        ,lines[current_line - USABLE_TER_ROW + 2].size());
        for (int i = current_line - USABLE_TER_ROW + 3; i <= current_line + 1; i++)
        {
	  write(STDOUT_FILENO, "\n\r\033[K", 5);
          write(STDOUT_FILENO, lines[i].c_str(), lines[i].size());
        }
        write(STDOUT_FILENO, "\r", 1);
      }
      current_line++;
      current_col = 0;
      break;
    }

  }
}
