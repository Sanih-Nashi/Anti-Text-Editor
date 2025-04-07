#include "antiutils.h"
#include "rawMode.h"
#include "keyProcess.h"
#include "terminal.h"

#include <iostream>

const float version = 1.0;

int main(int argc, char** argv) {
  RawMode::enableRawMode();
  utils::ClearScreen();
  initTerminal(&argv, argc);
  while (true){
   KeyPress::ProcessKeyPress();
  }
}
