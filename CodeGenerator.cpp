
#include "CodeGenerator.h"

CodeGenerator &CodeGenerator::getInstance() {
  static CodeGenerator instance;
  return instance;
}

void CodeGenerator::generate() const { this->generateBaseClass(); }

void CodeGenerator::generateBaseClass() const {
  std::ofstream fout;

  fout.open("generatedGame/BaseObjectClass.h");
  if (!fout) {
    std::cerr << "could not open file generatedGame/BaseObjectClass.h\n";
    std::exit(1);
  }

  fout << CodeGenerator::baseClassDotHCode;
  fout.close();

  fout.open("generatedGame/BaseObjectClass.cpp");
  if (!fout) {
    std::cerr << "could not open file generatedGame/BaseObjectClass.cpp\n";
    std::exit(1);
  }

  fout << CodeGenerator::baseClassDotCppCode;
  fout.close();
}

std::string CodeGenerator::baseClassDotHCode =
    R"(#ifndef __BASE_OBJECT_CLASS_H
#define __BASE_OBJECT_CLASS_H

#include <string>

#include "SDL2/SDL_keycode.h"

class BaseObjectClass {
public:
  // name of the object
  BaseObjectClass(const std::string &);

  virtual void createEvent();
  virtual void destroyEvent();
  
  virtual void stepEvent();  // runs before drawing


  // mouse events

  virtual void leftMouseDown();
  virtual void rightMouseDown();
  virtual void middleMouseDown();

  virtual void leftMouseUp();
  virtual void rightMouseUp();
  virtual void middleMouseUp();

  virtual void mouseEnter();
  virtual void mouseLeave();

  // end of mouse events

  // keyboard events

  virtual void keyDown();
  virtual void keyUp();

  // end of keyboard events

  // TODO add collision events

  virtual void keyDownEvent(SDL_Keycode);
  virtual void keyUpEvent();

private:
  std::string instanceName;
};

#endif // __BASE_OBJECT_CLASS_H
)";

std::string CodeGenerator::baseClassDotCppCode = R"(

#include "BaseObjectClass.h"

BaseObjectClass::BaseObjectClass(const std::string &instanceName) {
  this->instanceName = instanceName;
}

void BaseObjectClass::createEvent() {}
void BaseObjectClass::destroyEvent() {}

void BaseObjectClass::stepEvent() {}

void BaseObjectClass::leftMouseDown() {}
void BaseObjectClass::rightMouseDown() {}
void BaseObjectClass::middleMouseDown() {}

void BaseObjectClass::leftMouseUp() {}
void BaseObjectClass::rightMouseUp() {}
void BaseObjectClass::middleMouseUp() {}

void BaseObjectClass::mouseEnter() {}
void BaseObjectClass::mouseLeave() {}

void BaseObjectClass::keyDown() {}
void BaseObjectClass::keyUp() {}

void BaseObjectClass::keyDownEvent(SDL_Keycode) {}
void BaseObjectClass::keyUpEvent() {}

)";
