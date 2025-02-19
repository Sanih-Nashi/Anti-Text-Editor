#include <errno.h>

#include "KeyProcess.h"
#include "antiutils.h"


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
    write(STDOUT_FILENO, &c, 1);
  }

  switch(c){
    case CTRL_KEY('q'): 
    {
      
      write(STDOUT_FILENO, "\x1b[2J", 4);
      write(STDOUT_FILENO, "\x1b[H", 3);

      exit(0);
    }

    case DEL_KEY: 
    {
      write(STDOUT_FILENO, "\b \b", 3);
      break;
    }

  }
}
