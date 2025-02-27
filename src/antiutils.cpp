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
  std::ifstream file_in("test.txt");
  std::string buffer;
  for (const auto &pair : changes)
  {
    buffer = GetSpecificLine(pair.first + 1, file_in);
    buffer.insert(pair.second.first - 1, std::string(1, pair.second.second));   
    changed_lines[pair.first] = buffer;
    std::cout <<"\r\n" <<buffer <<", " <<pair.second.first - 1 <<"\r\n";
  }
  std::ofstream file_out("test.txt");
  
  for (int i = 1; std::getline(file_in, buffer); i++)
  {
    if (changed_lines.contains(i))
      file_out << changed_lines[i] <<"\n";
    else 
      file_out << buffer <<"\n";
  }	  

}

std::string utils::GetSpecificLine(const int& target_line, std::ifstream& file)
{
  if (!file.is_open())
  {
    std::cerr << "Error opening file"<< std::endl;
    return "";
  }

  std::string line;
  int current_line = 0;
  while (current_line <= target_line && std::getline(file, line))
  {

    if (changed_lines.contains(current_line))
      return changed_lines[current_line];
    else if (current_line == target_line)
      return line;
    
    current_line++;
  }

  std::cerr << "Line " << target_line << " not found. File has fewer lines." << "\n";
  return "";
}
