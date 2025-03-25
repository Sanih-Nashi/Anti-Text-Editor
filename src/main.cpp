#include "antiutils.h"
#include "RawMode.h"
#include "KeyProcess.h"
#include "terminal.h"

#include <iostream>

const float version = 1.0;

int main() {
  RawMode::enableRawMode();
  utils::ClearScreen();
  initTerminal();
  while (true){
   KeyPress::ProcessKeyPress();
  }
}
