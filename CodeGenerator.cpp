
#include "CodeGenerator.h"

CodeGenerator &CodeGenerator::getInstance() {
  static CodeGenerator instance;
  return instance;
}

void CodeGenerator::generate() const {
  this->copyRequiredClasses();
  this->generateMainCode();
  this->generateBaseClass();
  this->generateMakefile();
}

void CodeGenerator::copyRequiredClasses() const { std::system("cp Sprite.h Sprite.cpp generatedGame/"); }

void CodeGenerator::generateMainCode() const {
  std::ofstream fout;

  fout.open("generatedGame/main.cpp");
  if (!fout) {
    std::cerr << "could not open file generatedGame/main.cpp\n";
    std::exit(1);
  }

  fout << CodeGenerator::mainCode;
  fout.close();
}

void CodeGenerator::generateMakefile() const {
  std::ofstream fout;

  fout.open("generatedGame/Makefile");
  if (!fout) {
    std::cerr << "could not open file generatedGame/Makefile\n";
    std::exit(1);
  }

  fout << CodeGenerator::makefileCode;
  fout.close();
}

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

std::string CodeGenerator::makefileCode =
    R"(.PHONY: all run clean

all: build/main.o build/BaseObjectClass.o
	g++ -Ibuild build/*.o -o build/Game.out -Wall -g -O2 -std=c++2a -lSDL2 -lSDL2_ttf -lSDL2_image

run:
	./build/Game.out

build/main.o: main.cpp
	g++ -std=c++2a -c -IRUI -o build/main.o main.cpp

build/BaseObjectClass.o: BaseObjectClass.h BaseObjectClass.cpp
	g++ -std=c++2a -c -IRUI -o build/BaseObjectClass.o BaseObjectClass.cpp

clean:
	rm -rf build/*.o build/*.gch build/*.out
)";

std::string CodeGenerator::mainCode =
    R"(
#include <iostream>

#include "BaseObjectClass.h"

int main() {
  std::cout << "everything works!" << std::endl;

  return 0;
}
)";

std::string CodeGenerator::baseClassDotHCode =
    R"(#ifndef __BASE_OBJECT_CLASS_H
#define __BASE_OBJECT_CLASS_H

#include <string>
#include <utility>

#include "SDL2/SDL_keycode.h"

#include "Sprite.h"

class BaseObjectClass {
public:
  // name of the object
  BaseObjectClass(const std::string &, const std::string &, Sprite &, double, double);

  const std::string &getObjectName() const { return this->objectName; }
  const std::string &getInstanceName() const { return this->instanceName; }

  std::pair<double, double> getPosition() const { return {this->x, this->y}; }
  void setPosition(std::pair<double, double> newPosition) {
    this->x = newPosition.first;
    this->y = newPosition.second;
  }

  std::pair<double, double> getVelocity() const { return {this->vx, this->vy}; }
  void setVelocity(std::pair<double, double> newVelocity) {
    this->vx = newVelocity.first;
    this->vy = newVelocity.second;
  }

  virtual void createEvent();
  virtual void destroyEvent();

  virtual void stepEvent(); // runs before drawing

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

  virtual void collidedWith(std::shared_ptr<BaseObjectClass>);

  virtual void keyDownEvent(SDL_Keycode);
  virtual void keyUpEvent();

private:
  // TODO maybe change objectName to an enum
  std::string objectName;
  std::string instanceName;

  Sprite sprite;

  double x, y;
  double vx, vy;

  bool toBeDestroyed = false;
};

#endif // __BASE_OBJECT_CLASS_H
)";

std::string CodeGenerator::baseClassDotCppCode = R"(
#include "BaseObjectClass.h"

BaseObjectClass::BaseObjectClass(const std::string &objectName, const std::string &instanceName, Sprite &sprite,
                                 double x, double y) {
  this->objectName = objectName;
  this->instanceName = instanceName;
  this->sprite = sprite;
  this->x = x;
  this->y = y;
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

void BaseObjectClass::collidedWith(std::shared_ptr<BaseObjectClass>) {}

void BaseObjectClass::keyDownEvent(SDL_Keycode) {}
void BaseObjectClass::keyUpEvent() {}
)";
