#ifndef __CODE_GENERATOR_H
#define __CODE_GENERATIR_H

#include <cstdio>
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
  void generate(std::map<std::string, Room>, const std::string &) const;

private:
  CodeGenerator() {}

  void copyRequiredClasses() const;

  void generateMainCode(std::map<std::string, Room>, const std::string &) const;

  void generateMakefile(std::map<std::string, Room>) const;

  void generateBaseClass() const;
  void generateClassForObjects(std::map<std::string, Room>) const;

  void generateGameHandlerClass(std::map<std::string, Room>) const;
  const std::string generateGameHandlerIncludes(std::map<std::string, Room>) const;

  const std::string buildCreateObjectsSnippet(std::map<std::string, Room>) const;

  // some utils
  static std::set<std::string> extractObjectNamesFromRooms(std::map<std::string, Room>);
  static void replaceString(std::string &, const std::string &, const std::string &);
  static std::string vectorToString(std::vector<std::string>);
  static std::string buildMakefileTargetsForObjects(std::set<std::string>);
  static std::string buildMakefileBuildCommandsForObjects(std::set<std::string>);

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
