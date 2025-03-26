#include "antiutils.h"
#include "rawMode.h"
#include "keyProcess.h"
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
