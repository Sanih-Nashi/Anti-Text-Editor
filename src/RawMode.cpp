#include "RawMode.h"
#include "antiutils.h"

void RawMode::disableRawMode(){
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH ,&ter.orig_setting) == -1)
    utils::die("tcsetattr");
}


void RawMode::enableRawMode() {

  if (tcgetattr(STDIN_FILENO, &ter.orig_setting) == -1) 
    utils::die("tcgetattr");
  atexit(disableRawMode);
  struct termios raw = ter.orig_setting;

  raw.c_lflag &= ~(ECHO | ISIG | IEXTEN | ICANON);
  raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  raw.c_oflag &= ~(OPOST);
  raw.c_cflag |= (CS8);

  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 1;

  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
    utils::die("tcsetattr");
}


