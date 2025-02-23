#include "antiutils.h"
#include "terminal.h"

#include <string>
#include <iostream>
#include <fstream>

void utils::die(const char *errormsg)
{

  write(STDOUT_FILENO, "\x1b[2J", 4);
  write(STDOUT_FILENO, "\x1b[H", 3);

  perror(errormsg);
  exit(1);
}

void utils::ClearScreen()
{
  write(STDOUT_FILENO, "\x1b[2J", 4); // 1b is hexadecimal for 27 and '[' which is a start of an escape sequence
  // J means to erase and '2' is the argument which means clear the entire screen}
  write(STDOUT_FILENO, "\x1b[H", 3); // H for moving the cursor, here to the beginning
}

void utils::CommitChanges()
{
  std::string buffer;
  for (const auto &pair : changes)
  {
    buffer = GetSpecificLine("test.txt", pair.first);
    buffer.insert(pair.first, reinterpret_cast<const char*>(&pair.second.second));
  }
}

std::string utils::GetSpecificLine(const std::string &file_path, int target_line)
{
  std::ifstream file(file_path);
  if (!file.is_open())
  {
    std::cerr << "Error opening file: " << file_path << std::endl;
    return "";
  }

  std::string line;
  int current_line = 1;
  while (current_line <= target_line && std::getline(file, line))
  {
    if (current_line == target_line)
    {
      return line;
    }
    current_line++;
  }

  std::cerr << "Line " << target_line << " not found. File has fewer lines." << "\n";
  return "";
}