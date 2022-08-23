#ifndef __CODE_GENERATOR_H
#define __CODE_GENERATIR_H

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
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
  void generate(std::map<std::string, Room>) const;

private:
  CodeGenerator() {}

  void copyRequiredClasses() const;

  void generateMainCode() const;
  void generateMakefile() const;

  void generateBaseClass() const;
  void generateClassForObjects(std::map<std::string, Room>) const;

  void generateGameHandlerClass() const;

  // some utils
  static std::set<std::string> extractObjectNamesFromRooms(std::map<std::string, Room>);
  static void replaceString(std::string &, const std::string &, const std::string &);

  std::string folderName;

  static std::string makefileCode;

  static std::string mainCode;

  static std::string baseClassDotHCode;
  static std::string baseClassDotCppCode;

  static std::string objectClassDotHCode;
  static std::string objectClassDotCppCode;

  static std::string gameHandlerDotHCode;
  static std::string gameHandlerDotCppCode;
};

#endif // __CODE_GENERATOR_H
