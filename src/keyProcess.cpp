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


RepeatKeyProcessing:

  if (31 < c && c < 127)
  {
    current_row++;
    lines[current_line].insert(current_row - 1, std::string(1, c));
    write(STDOUT_FILENO, "\r", 1);
    write(STDOUT_FILENO, lines[current_line].c_str(), lines[current_line].size());
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
      c = utils::CommitChanges();
      if (c != -1)
        goto RepeatKeyProcessing;
      break;
    }

    case CTRL_KEY('k'): 
    {
      if (current_col > 0)
      {
        if (current_row <= lines[current_col - 1].size())
          write(STDOUT_FILENO, "\033[A", 3);

        else
        {
	  char str[11];
	  int size = lines[current_line - 1].size();
	  int len = snprintf(str, sizeof(str), "\033[A\r\033[%dC", size);
          write(STDOUT_FILENO, str, len);
          current_row = size;
        }

        current_col--;
        current_line--;
      }
      
      else if (current_col == 0 && current_line > 0)
      {
	write(STDOUT_FILENO, "\033[H", 3);
        for (int i = --current_line; i < current_line + USABLE_TER_COL; i++)
	{
          write(STDOUT_FILENO, "\033[K", 3);
	  write(STDOUT_FILENO, lines[i].c_str(), lines[i].size());
	}
	write(STDOUT_FILENO, "\033[H", 3);
	if (current_row != 0)
	{
	  char str[22];
	  int len = snprintf(str, sizeof(str), "\033[%dC", current_row);
	}
      }
	  	
      break;
    }
    case CTRL_KEY('j'): 
    {

      if (current_col < USABLE_TER_COL && current_line < lines.size() - 1)
      {
        
        if (current_row <= lines[current_line + 1].size()) 
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

	  current_row = lines[current_col + 1].size();
        }
        current_col++;
        current_line++;
      }

      else if (current_col == USABLE_TER_COL && current_line < lines.size() -1)
      {
	write(STDOUT_FILENO, "\033[H", 3);
	for (int i = ++current_line - USABLE_TER_COL + 1; i <= current_line; i++)
	{
	  write(STDOUT_FILENO, "\033[K", 3);
	  write(STDOUT_FILENO, lines[i].c_str(), lines[i].size()); 
	}
	if (current_col != 0)
	{
	  char str[22];
	  int len = snprintf(str, sizeof(str), "\033[%dC", current_col);
	  write(STDOUT_FILENO, str, len);
        }
	else
	  write(STDOUT_FILENO, "\r", 1);

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
        lines[current_line].erase(--current_row, 1);
        write(STDOUT_FILENO, "\r\033[K", 4);
        write(STDOUT_FILENO, lines[current_line].c_str(), lines[current_line].size());
  	    write(STDOUT_FILENO, "\r", 1);
	      if (current_row != 0){
          // std::string row = "\033[" + std::to_string(current_row) + "C";
          char str[7];
          int len = snprintf(str, sizeof(str), "\033[%dC", current_row); 
          write(STDOUT_FILENO, str, len);
        } 	
      }

      else
      {  
        if (current_col != 0)
        {
          int size = lines[current_line - 1].size();
          // std::string num = "\033[" + std::to_string(size) + "C";
          
	  lines[current_line - 1] = lines[current_line - 1] + lines[current_line];
          lines.erase(lines.begin() + current_line--);
	  current_col--;
          current_row = size;

          write(STDOUT_FILENO, "\033[A", 3);
          write(STDOUT_FILENO, lines[current_line].c_str(), lines[current_line].size());

          for (int i = current_col + 1; i < lines.size(); i++){
            write(STDOUT_FILENO, "\n\r\033[K", 5);
            write(STDOUT_FILENO, lines[i].c_str(), lines[i].size());
          }      

          // std::string num2 = "\033[" + std::to_string((lines.size() + 1) - current_col) + "A";
          write(STDOUT_FILENO, "\n\r\033[K\033[H", 8);
	  if (current_col != 0 )
	  {
            char str2[7];
            int len2 = snprintf(str2, sizeof(str2), "\033[%dB", current_col);  
	    write(STDOUT_FILENO, str2, len2);
	  }
	  if (current_row != 0)
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
      lines.insert(lines.begin() + current_line, lines[current_line].substr(0, current_row));
      lines[current_line + 1] = lines[current_line + 1].substr(current_row, lines[current_line + 1].size() - 1);

      write(STDOUT_FILENO, "\033[K", 3);
      for (int i = current_line + 1; i < (current_line + 1) + (USABLE_TER_COL - current_col) && i < lines.size(); i++)
      {
        write(STDOUT_FILENO, "\n\r\033[K", 5);
        write(STDOUT_FILENO, lines[i].c_str(), lines[i].size());
      }        
      // std::string num = "\033[" + std::to_string(lines.size() - current_col -1) + "A";
      char str[7];
      int len = snprintf(str, sizeof(str), "\033[%dA", lines.size() - current_line - 1); 
      write(STDOUT_FILENO, "\n\r\033[K", 5);
      write(STDOUT_FILENO, str, len);
      current_col++;
      current_line++;
      current_row = 0;
      break;
    }
      
  }
}
