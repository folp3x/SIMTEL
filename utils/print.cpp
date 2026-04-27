#include "print.h"

#include <iostream>

// выводит текст цветом color
void printColored(const std::string &output, rang::fg color) {
  std::cout << color << output << rang::fg::reset;
}

// выводит текст цветом color и добавляет '\n'
void printlnColored(const std::string &output, rang::fg color) {
  printColored(output, color);
  std::cout << std::endl;
}
