
#include "IdGenerator.h"

#include <iostream>

int IdGenerator::currentIndex = 28;

std::string IdGenerator::getId() {
  int temp = IdGenerator::currentIndex;
  std::string returnValue = "";
  while (temp > 0) {
    int mod = temp % 27;
    returnValue.insert(0, 1, 'a' - 1 + mod);
    temp = temp / 27;
  }
  IdGenerator::currentIndex++;
  if (IdGenerator::currentIndex % 27 == 0)
    IdGenerator::currentIndex++;

  return returnValue;
}
