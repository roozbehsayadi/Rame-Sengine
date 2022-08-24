
#include "CodeGenerator.h"

CodeGenerator &CodeGenerator::getInstance() {
  static CodeGenerator instance;
  return instance;
}

void CodeGenerator::generate(std::map<std::string, Room> rooms, const std::string &firstRoom) const {
  this->copyRequiredClasses();
  this->generateMainCode(rooms, firstRoom);
  this->generateClassForObjects(rooms);
  this->generateGameHandlerClass(rooms);
  this->generateBaseClass();
  this->generateMakefile();

  // this->buildCreateObjectsSnippet(rooms);
}

void CodeGenerator::copyRequiredClasses() const {
  std::system("cp -r Sprite.* data_types/ RUI/monitor/ RUI/utils/ assets/ generatedGame/");
}

void CodeGenerator::generateMainCode(std::map<std::string, Room> rooms, const std::string &firstRoom) const {
  std::ofstream fout;

  fout.open("generatedGame/main.cpp");
  if (!fout) {
    std::cerr << "could not open file generatedGame/main.cpp\n";
    std::exit(1);
  }

  std::string mainTemp = CodeGenerator::mainCode;
  auto initializationCommands = this->buildCreateObjectsSnippet(rooms);
  CodeGenerator::replaceString(mainTemp, "${INCLUDE_ALL_OBJECTS}", generateGameHandlerIncludes(rooms));
  CodeGenerator::replaceString(mainTemp, "${LOOP_AND_CREATE_ALL_OBJECT_INSTANCES}", initializationCommands);
  CodeGenerator::replaceString(mainTemp, "${FIRST_ROOM_NAME}", firstRoom);
  fout << mainTemp;
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

void CodeGenerator::generateClassForObjects(std::map<std::string, Room> rooms) const {
  auto objectNames = CodeGenerator::extractObjectNamesFromRooms(rooms);
  for (auto objectName : objectNames) {
    std::string classDotHCode = CodeGenerator::objectClassDotHCode;
    CodeGenerator::replaceString(classDotHCode, "${OBJECT_NAME}", objectName);
    std::string classDotCppCode = CodeGenerator::objectClassDotCppCode;
    CodeGenerator::replaceString(classDotCppCode, "${OBJECT_NAME}", objectName);

    std::ofstream fout;

    fout.open("generatedGame/" + objectName + "_ObjectClass.h");
    if (!fout) {
      std::cerr << "could not open file generatedGame/" + objectName + "_ObjectClass.h";
      std::exit(1);
    }

    fout << classDotHCode;
    fout.close();

    fout.open("generatedGame/" + objectName + "_ObjectClass.cpp");
    if (!fout) {
      std::cerr << "could not open file generatedGame/" + objectName + "_ObjectClass.cpp";
      std::exit(1);
    }

    fout << classDotCppCode;
    fout.close();
  }
}

void CodeGenerator::generateGameHandlerClass(std::map<std::string, Room> rooms) const {
  std::ofstream fout;

  fout.open("generatedGame/GameHandler.h");
  if (!fout) {
    std::cerr << "could not open file generatedGame/GameHandler.h\n";
    std::exit(1);
  }

  std::string dotHTemp = CodeGenerator::gameHandlerDotHCode;
  CodeGenerator::replaceString(dotHTemp, "${INCLUDE_ALL_OBJECTS}", generateGameHandlerIncludes(rooms));
  fout << dotHTemp;
  fout.close();

  fout.open("generatedGame/GameHandler.cpp");
  if (!fout) {
    std::cerr << "could not open file generatedGame/GameHandler.cpp\n";
    std::exit(1);
  }

  fout << CodeGenerator::gameHandlerDotCppCode;
  fout.close();
}

const std::string CodeGenerator::generateGameHandlerIncludes(std::map<std::string, Room> rooms) const {
  std::set<std::string> objectNames;
  for (auto [roomName, room] : rooms) {
    auto objects = room.getObjects();
    for (auto object : objects) {
      objectNames.insert(object->getName());
    }
  }
  std::string returnValue = "";
  for (auto objectName : objectNames)
    returnValue += "\n#include \"" + objectName + "_ObjectClass.h\"";

  return returnValue + "\n";
}

const std::string CodeGenerator::buildCreateObjectsSnippet(std::map<std::string, Room> rooms) const {
  std::string oneIterationTemplate = R"(auto ${INSTANCE_NAME} = std::make_shared<${OBJECT_NAME}_ObjectClass>(
      "${OBJECT_NAME}", "${INSTANCE_NAME}", Sprite("${SPRITE_NAME}", ${SPRITE_FPS}, ${SPRITE_FRAMES}), ${OBJECT_X}, ${OBJECT_Y});
  gameHandler.addObject("${ROOM_NAME}", ${INSTANCE_NAME});
)";
  std::string returnValue = "";
  for (auto [roomName, room] : rooms)
    for (auto object : room.getObjects()) {
      auto temp = oneIterationTemplate;
      CodeGenerator::replaceString(temp, "${OBJECT_NAME}", object->getName());
      CodeGenerator::replaceString(temp, "${INSTANCE_NAME}", object->getInstanceName());
      CodeGenerator::replaceString(temp, "${SPRITE_NAME}", object->getSprite().getName());
      CodeGenerator::replaceString(temp, "${SPRITE_FPS}", std::to_string(object->getSprite().getFPS()));
      CodeGenerator::replaceString(temp, "${SPRITE_FRAMES}",
                                   CodeGenerator::vectorToString(object->getSprite().getFramePaths()));
      CodeGenerator::replaceString(temp, "${OBJECT_X}", std::to_string(object->getX()));
      CodeGenerator::replaceString(temp, "${OBJECT_Y}", std::to_string(object->getY()));
      CodeGenerator::replaceString(temp, "${ROOM_NAME}", roomName);
      returnValue += temp;
    }
  return returnValue;
}

std::set<std::string> CodeGenerator::extractObjectNamesFromRooms(std::map<std::string, Room> rooms) {
  std::set<std::string> returnValue = {};
  for (auto [roomName, room] : rooms) {
    auto roomObjects = room.getObjects();
    for (auto &object : roomObjects)
      returnValue.insert(object->getName());
  }
  return returnValue;
}

void CodeGenerator::replaceString(std::string &text, const std::string &from, const std::string &to) {
  if (from.empty())
    return;
  size_t startPos = 0;
  while ((startPos = text.find(from, startPos)) != std::string::npos) {
    text.replace(startPos, from.length(), to);
    startPos += to.length();
  }
}

std::string CodeGenerator::vectorToString(std::vector<std::string> input) {
  if (input.empty())
    return "";

  if (input.size() == 1u)
    return "{\"" + input.at(0) + "\"}";

  std::string returnValue = "{\"" + input.at(0) + "\"";
  for (int i = 1; i < input.size(); i++)
    returnValue += ", \"" + input.at(i) + "\"";
  returnValue += "}";
  return returnValue;
}

std::string CodeGenerator::makefileCode =
    R"(.PHONY: all run clean

all: build/main.o build/BaseObjectClass.o build/GameHandler.o build/Sprite.o build/Image.o build/RuiMonitor.o build/Color.o build/Rect.o build/Geometry.o
	g++ -Ibuild build/*.o -o build/Game.out -Wall -g -O2 -std=c++2a -lSDL2 -lSDL2_ttf -lSDL2_image

run:
	gdb -q build/Game.out

build/main.o: main.cpp
	g++ -std=c++2a -c -I. -o build/main.o main.cpp

build/BaseObjectClass.o: BaseObjectClass.h BaseObjectClass.cpp
	g++ -std=c++2a -c -I. -o build/BaseObjectClass.o BaseObjectClass.cpp

build/GameHandler.o: GameHandler.h GameHandler.cpp
	g++ -std=c++2a -c -I. -o build/GameHandler.o GameHandler.cpp

build/Sprite.o: Sprite.h Sprite.cpp
	g++ -std=c++2a -c -I. -o build/Sprite.o Sprite.cpp

build/Image.o: data_types/Image.h data_types/Image.cpp
	g++ -std=c++2a -c -I. -o build/Image.o data_types/Image.cpp

build/RuiMonitor.o: monitor/RuiMonitor.h monitor/RuiMonitor.cpp
	g++ -std=c++2a -c -I. -o build/RuiMonitor.o monitor/RuiMonitor.cpp

build/Color.o: utils/Color.h utils/Color.cpp
	g++ -std=c++2a -c -I. -o build/Color.o utils/Color.cpp

build/Rect.o: utils/Rect.h utils/Rect.cpp
	g++ -std=c++2a -c -I. -o build/Rect.o utils/Rect.cpp

build/Geometry.o: utils/Geometry.h utils/Geometry.cpp
	g++ -std=c++2a -c -I. -o build/Geometry.o utils/Geometry.cpp

clean:
	rm -rf build/*.o build/*.gch build/*.out
)";

std::string CodeGenerator::mainCode =
    R"(
#include <iostream>

#include "GameHandler.h"
#include "monitor/RuiMonitor.h"

#include "BaseObjectClass.h"
${INCLUDE_ALL_OBJECTS}

int main() {
  RuiMonitor monitor("Game");
  GameHandler gameHandler(monitor);

  // create object instances
  std::cerr << "creating object instances\n";
  ${LOOP_AND_CREATE_ALL_OBJECT_INSTANCES}
  std::cerr << "object instances created\n";
  // end of creating object instances

  gameHandler.setCurrentRoom("${FIRST_ROOM_NAME}");

  std::cerr << "starting the game\n";
  gameHandler.start();

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

class GameHandler;

class BaseObjectClass {
  friend class GameHandler;

public:
  // name of the object
  BaseObjectClass(const std::string &, const std::string &, const Sprite &, double, double);

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

BaseObjectClass::BaseObjectClass(const std::string &objectName, const std::string &instanceName, const Sprite &sprite,
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

std::string CodeGenerator::objectClassDotHCode =
    R"(#ifndef __${OBJECT_NAME}_OBJECT_CLASS_H
#define __${OBJECT_NAME}_OBJECT_CLASS_H

#include <string>
#include <utility>

#include "SDL2/SDL_keycode.h"

#include "BaseObjectClass.h"
#include "Sprite.h"

class ${OBJECT_NAME}_ObjectClass : public BaseObjectClass {
public:
  using BaseObjectClass::BaseObjectClass;

  // override events here by using BaseObjectClass's virtual functions

private:
};

#endif // __${OBJECT_NAME}_OBJECT_CLASS_H
)";

std::string CodeGenerator::objectClassDotCppCode = R"(
#include "${OBJECT_NAME}_ObjectClass.h"

// override events here by using BaseObjectClass's virtual functions
)";

std::string CodeGenerator::gameHandlerDotHCode =
    R"(#ifndef __GAME_HANDLER_H
#define __GAME_HANDLER_H

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "BaseObjectClass.h"
#include "monitor/RuiMonitor.h"
#include "utils/Rect.h"

class GameHandler {
public:
  GameHandler(RuiMonitor &monitor) : monitor(monitor) {}
  
  void start();

  // Arguments:
  // - room name
  // - the object itself
  void addObject(const std::string &, std::shared_ptr<BaseObjectClass>);

  void render(std::shared_ptr<BaseObjectClass>);

  void setCurrentRoom(const std::string &);

private:
  // room name -> its objects
  std::map<std::string, std::vector<std::shared_ptr<BaseObjectClass>>> gameObjects;

  RuiMonitor monitor;
  std::string currentRoom = "";
};

#endif // __GAME_HANDLER_H
)";

std::string CodeGenerator::gameHandlerDotCppCode =
    R"(
#include "GameHandler.h"

void GameHandler::start() {
  monitor.clear({0, 0, 0, 255});
  while (true) {
    for (auto pairItr : gameObjects) {
      auto roomName = pairItr.first;
      if (roomName == this->currentRoom) {
        auto &objects = pairItr.second;
        for (auto &object : objects) {
          this->render(object);
        }
      }
    }
    monitor.update();
  }
}

void GameHandler::addObject(const std::string &roomName, std::shared_ptr<BaseObjectClass> object) {
  if (!gameObjects.contains(roomName))
    gameObjects.insert({roomName, {}});
  gameObjects.find(roomName)->second.push_back(object);
}

void GameHandler::render(std::shared_ptr<BaseObjectClass> object) {
  auto currentFrame = object->sprite.getCurrentFrameAndProceed();
  auto windowSize = monitor.getMonitorSize();
  monitor.drawImage({object->getPosition().first, object->getPosition().second, -1, -1},
                    {0, 0, double(windowSize.first), double(windowSize.second)}, currentFrame->getTexture(),
                    currentFrame->getImagePath());
}

void GameHandler::setCurrentRoom(const std::string &roomName) { this->currentRoom = roomName; }
)";
