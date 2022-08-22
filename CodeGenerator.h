#ifndef __CODE_GENERATOR_H
#define __CODE_GENERATIR_H

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "Room.h"

// TODO pass the rooms
class CodeGenerator {
public:
  // name of generated codes folder + list of rooms
  CodeGenerator(const std::string &, std::vector<Room>);

  void generate() const;

private:
  void generateBaseClass() const;

  std::string folderName;

  static std::string baseClassDotHCode;
  static std::string baseClassDotCppCode;
};

#endif // __CODE_GENERATOR_H
