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
  static CodeGenerator &getInstance();

  CodeGenerator(CodeGenerator const &) = delete;
  void operator=(CodeGenerator const &) = delete;

  // TDOO add list of rooms as input of this function
  void generate() const;

private:
  CodeGenerator() {}

  void generateBaseClass() const;

  std::string folderName;

  static std::string baseClassDotHCode;
  static std::string baseClassDotCppCode;
};

#endif // __CODE_GENERATOR_H
