
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
  this->generateBaseClass(rooms);
  this->generateMakefile(rooms);
  this->generateGlobalVariablesClass(rooms);
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
  CodeGenerator::replaceString(mainTemp, "${INCLUDE_ALL_OBJECTS}", generateAllObjectsIncludes(rooms));
  CodeGenerator::replaceString(mainTemp, "${LOOP_AND_CREATE_ALL_OBJECT_INSTANCES}", initializationCommands);
  CodeGenerator::replaceString(mainTemp, "${FIRST_ROOM_NAME}", firstRoom);
  fout << mainTemp;
  fout.close();
}

void CodeGenerator::generateMakefile(std::map<std::string, Room> rooms) const {
  std::ofstream fout;

  fout.open("generatedGame/Makefile");
  if (!fout) {
    std::cerr << "could not open file generatedGame/Makefile\n";
    std::exit(1);
  }

  std::string temp = CodeGenerator::makefileCode;
  auto objectNames = CodeGenerator::extractObjectNamesFromRooms(rooms);

  CodeGenerator::replaceString(temp, "${OBJECTS_OBJECT_FILES}",
                               CodeGenerator::buildMakefileTargetsForObjects(objectNames));
  CodeGenerator::replaceString(temp, "${BUILD_OBJECTS_OBJECT_FILES}",
                               CodeGenerator::buildMakefileBuildCommandsForObjects(objectNames));

  fout << temp;
  fout.close();
}

void CodeGenerator::generateBaseClass(std::map<std::string, Room> rooms) const {
  std::ofstream fout;

  fout.open("generatedGame/BaseObjectClass.h");
  if (!fout) {
    std::cerr << "could not open file generatedGame/BaseObjectClass.h\n";
    std::exit(1);
  }

  std::string temp = CodeGenerator::baseClassDotHCode;
  CodeGenerator::replaceString(temp, "${OBJECTS_CLASSES}",
                               buildAllObjectsForwardDeclarations(this->extractObjectNamesFromRooms(rooms)));
  CodeGenerator::replaceString(temp, "${FRIEND_OBJECT_CLASSES}",
                               buildAllObjectsAsFriends(this->extractObjectNamesFromRooms(rooms)));
  fout << temp;
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

void CodeGenerator::generateGlobalVariablesClass(std::map<std::string, Room> rooms) const {
  std::ofstream fout;

  fout.open("generatedGame/GlobalVariables.h");
  if (!fout) {
    std::cerr << "could not open file generatedGame/GlobalVariables.h\n";
    std::exit(1);
  }

  std::string dotHTemp = CodeGenerator::globalVariablesDotHCode;
  CodeGenerator::replaceString(dotHTemp, "${SPRITES}", buildSpriteStaticVariables(rooms));

  fout << dotHTemp;
  fout.close();

  fout.open("generatedGame/GlobalVariables.cpp");
  if (!fout) {
    std::cerr << "could not open file generatedGame/GlobalVariables.cpp\n";
    std::exit(1);
  }

  std::string temp = CodeGenerator::globalVariablesDotCppCode;
  CodeGenerator::replaceString(temp, "${SPRITES}", buildSpriteStaticVariableInitializations(rooms));
  fout << temp;
  fout.close();
}

void CodeGenerator::generateGameHandlerClass(std::map<std::string, Room> rooms) const {
  std::ofstream fout;

  fout.open("generatedGame/GameHandler.h");
  if (!fout) {
    std::cerr << "could not open file generatedGame/GameHandler.h\n";
    std::exit(1);
  }

  std::string dotHTemp = CodeGenerator::gameHandlerDotHCode;
  CodeGenerator::replaceString(dotHTemp, "${INCLUDE_ALL_OBJECTS}", generateAllObjectsIncludes(rooms));
  CodeGenerator::replaceString(
      dotHTemp, "${CREATE_FUNCTIONS_FOR_ALL_OBJECTS}",
      generateGameHandlerCreateObjectDefinitions(CodeGenerator::extractObjectNamesFromRooms(rooms)));
  fout << dotHTemp;
  fout.close();

  fout.open("generatedGame/GameHandler.cpp");
  if (!fout) {
    std::cerr << "could not open file generatedGame/GameHandler.cpp\n";
    std::exit(1);
  }

  std::string temp = CodeGenerator::gameHandlerDotCppCode;
  CodeGenerator::replaceString(temp, "${CREATE_OBJECT_FUNCTIONS_IMPLEMENTATIONS}",
                               generateCreateFunctionForEachObject(CodeGenerator::extractObjectNamesFromRooms(rooms)));

  fout << temp;
  fout.close();
}

const std::string CodeGenerator::generateGameHandlerCreateObjectDefinitions(std::set<std::string> objectNames) const {
  std::string returnValue = "";
  std::string functionTemplate =
      R"(  void add${OBJECT_NAME}_ObjectClass(const std::string &, const std::string &, const Sprite &, double, double);
)";
  for (auto &objectName : objectNames) {
    std::string temp = functionTemplate;
    CodeGenerator::replaceString(temp, "${OBJECT_NAME}", objectName);
    returnValue += temp;
  }
  return returnValue;
}

const std::string CodeGenerator::generateCreateFunctionForEachObject(std::set<std::string> objectNames) const {
  std::string
      returnValue = "",
      functionTemplate =
          R"(void GameHandler::add${OBJECT_NAME}_ObjectClass(const std::string &roomName, const std::string &instanceName, const Sprite &sprite, double x, double y) {
  std::shared_ptr<${OBJECT_NAME}_ObjectClass> temp = std::make_shared<${OBJECT_NAME}_ObjectClass>(
    "${OBJECT_NAME}", instanceName, sprite, x, y);
  this->addObject(roomName, temp);
}
)";

  for (auto &objectName : objectNames) {
    std::string temp = functionTemplate;
    CodeGenerator::replaceString(temp, "${OBJECT_NAME}", objectName);
    returnValue += temp;
  }

  return returnValue;
}

const std::string CodeGenerator::generateAllObjectsIncludes(std::map<std::string, Room> rooms) const {
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

const std::string CodeGenerator::buildSpriteStaticVariables(std::map<std::string, Room> rooms) const {
  std::string returnValue = "";
  std::string variableTemplate =
      R"(  static const Sprite ${SPRITE_NAME};
)";

  std::set<std::string> spriteNames;
  for (auto &itr : rooms) {
    auto objects = itr.second.getObjects();
    for (auto &object : objects)
      spriteNames.insert(object->getSprite().getName());
  }

  for (auto spriteName : spriteNames) {
    std::string temp = variableTemplate;
    CodeGenerator::replaceString(temp, "${SPRITE_NAME}", spriteName);
    returnValue += temp;
  }

  return returnValue;
}

const std::string CodeGenerator::buildSpriteStaticVariableInitializations(std::map<std::string, Room> rooms) const {
  std::string returnValue = "";
  std::string variableTemplate =
      R"(const Sprite GlobalVariables::${SPRITE_NAME} = Sprite("${SPRITE_NAME}", ${SPRITE_FPS}, ${SPRITE_FRAMES});
)";

  std::set<std::string> spriteNames;
  for (auto &itr : rooms) {
    auto objects = itr.second.getObjects();
    for (auto &object : objects) {
      if (!spriteNames.contains(object->getSprite().getName())) {
        std::string temp = variableTemplate;
        CodeGenerator::replaceString(temp, "${SPRITE_NAME}", object->getSprite().getName());
        CodeGenerator::replaceString(temp, "${SPRITE_FPS}", std::to_string(object->getSprite().getFPS()));
        CodeGenerator::replaceString(temp, "${SPRITE_FRAMES}",
                                     CodeGenerator::vectorToString(object->getSprite().getFramePaths()));
        returnValue += temp;

        spriteNames.insert(object->getSprite().getName());
      }
    }
  }

  return returnValue;
}

const std::string CodeGenerator::buildCreateObjectsSnippet(std::map<std::string, Room> rooms) const {
  std::string oneIterationTemplate = R"(auto ${INSTANCE_NAME} = std::make_shared<${OBJECT_NAME}_ObjectClass>(
      "${OBJECT_NAME}", "${INSTANCE_NAME}", GlobalVariables::${SPRITE_NAME}, ${OBJECT_X}, ${OBJECT_Y});
  gameHandler.addObject("${ROOM_NAME}", ${INSTANCE_NAME});
)";
  std::string returnValue = "";
  for (auto [roomName, room] : rooms)
    for (auto object : room.getObjects()) {
      auto temp = oneIterationTemplate;
      CodeGenerator::replaceString(temp, "${OBJECT_NAME}", object->getName());
      CodeGenerator::replaceString(temp, "${INSTANCE_NAME}", object->getInstanceName());
      CodeGenerator::replaceString(temp, "${SPRITE_NAME}", object->getSprite().getName());
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

std::string CodeGenerator::buildMakefileTargetsForObjects(std::set<std::string> classes) {
  std::string returnValue = "";
  for (auto className : classes)
    returnValue += "build/" + className + "_ObjectClass.o ";

  return returnValue;
}

std::string CodeGenerator::buildMakefileBuildCommandsForObjects(std::set<std::string> classes) {
  std::string returnValue = "";
  std::string singleTarget =
      R"(build/${FILE_NAME}.o: ${FILE_NAME}.h ${FILE_NAME}.cpp
	g++ -std=c++2a -c -I. -o build/${FILE_NAME}.o ${FILE_NAME}.cpp
)";
  for (auto className : classes) {
    std::string temp = singleTarget;
    std::string fileName = className + "_ObjectClass";
    CodeGenerator::replaceString(temp, "${FILE_NAME}", fileName);
    returnValue += temp;
  }

  return returnValue;
}

const std::string CodeGenerator::buildAllObjectsForwardDeclarations(std::set<std::string> objectNames) const {
  std::string returnValue = "";
  std::string classTemplate =
      R"(class ${CLASS_NAME};
)";
  for (auto objectName : objectNames) {
    std::string temp = classTemplate;
    CodeGenerator::replaceString(temp, "${CLASS_NAME}", objectName);
    returnValue += temp;
  }

  return returnValue;
}

const std::string CodeGenerator::buildAllObjectsAsFriends(std::set<std::string> objectNames) const {
  std::string returnValue = "";
  std::string classTemplate =
      R"(friend class ${CLASS_NAME};
)";
  for (auto objectName : objectNames) {
    std::string temp = classTemplate;
    CodeGenerator::replaceString(temp, "${CLASS_NAME}", objectName);
    returnValue += temp;
  }

  return returnValue;
}

std::string CodeGenerator::makefileCode =
    R"(.PHONY: all run clean

all: build/main.o build/BaseObjectClass.o build/GameHandler.o build/Sprite.o build/Image.o build/RuiMonitor.o build/Color.o build/Rect.o build/Geometry.o build/GlobalVariables.o ${OBJECTS_OBJECT_FILES}
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

build/GlobalVariables.o: GlobalVariables.h GlobalVariables.cpp
	g++ -std=c++2a -c -I. -o build/GlobalVariables.o GlobalVariables.cpp

${BUILD_OBJECTS_OBJECT_FILES}
clean:
	rm -rf build/*.o build/*.gch build/*.out
)";

std::string CodeGenerator::mainCode =
    R"(
#include <iostream>

#include "GameHandler.h"
#include "GlobalVariables.h"
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
${OBJECTS_CLASSES}

class BaseObjectClass {
  friend class GameHandler;
  ${FRIEND_OBJECT_CLASSES}

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

  virtual void createEvent(GameHandler &);
  virtual void destroyEvent(GameHandler &);

  virtual void stepEvent(GameHandler &); // runs before drawing

  // mouse events

  virtual void leftMouseDown(GameHandler &);
  virtual void rightMouseDown(GameHandler &);
  virtual void middleMouseDown(GameHandler &);

  virtual void leftMouseUp(GameHandler &);
  virtual void rightMouseUp(GameHandler &);
  virtual void middleMouseUp(GameHandler &);

  virtual void mouseMove(GameHandler &);

  virtual void mouseEnter(GameHandler &);
  virtual void mouseLeave(GameHandler &);

  // end of mouse events

  // keyboard events

  virtual void keyDown(GameHandler &);
  virtual void keyUp(GameHandler &);

  // end of keyboard events

  virtual void collidedWith(std::shared_ptr<BaseObjectClass>, GameHandler &);

protected:
  // TODO maybe change objectName to an enum
  std::string objectName;
  std::string instanceName;

  Sprite sprite;

  double x, y;
  double vx = 0, vy = 0;

  bool toBeDestroyed = false;
  // only use it in keyboard event functions
  SDL_Keycode keyboardEventValue = SDLK_UNKNOWN;

  bool isMouseInside = false;
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

void BaseObjectClass::createEvent(GameHandler &) {}
void BaseObjectClass::destroyEvent(GameHandler &) {}

void BaseObjectClass::stepEvent(GameHandler &) {}

void BaseObjectClass::leftMouseDown(GameHandler &) {}
void BaseObjectClass::rightMouseDown(GameHandler &) {}
void BaseObjectClass::middleMouseDown(GameHandler &) {}

void BaseObjectClass::leftMouseUp(GameHandler &) {}
void BaseObjectClass::rightMouseUp(GameHandler &) {}
void BaseObjectClass::middleMouseUp(GameHandler &) {}

void BaseObjectClass::mouseMove(GameHandler &) {}

void BaseObjectClass::mouseEnter(GameHandler &) {}
void BaseObjectClass::mouseLeave(GameHandler &) {}

void BaseObjectClass::keyDown(GameHandler &) {}
void BaseObjectClass::keyUp(GameHandler &) {}

void BaseObjectClass::collidedWith(std::shared_ptr<BaseObjectClass>, GameHandler &) {}
)";

std::string CodeGenerator::objectClassDotHCode =
    R"(#ifndef __${OBJECT_NAME}_OBJECT_CLASS_H
#define __${OBJECT_NAME}_OBJECT_CLASS_H

#include <string>
#include <utility>

#include "SDL2/SDL_keycode.h"

#include "BaseObjectClass.h"
#include "GameHandler.h"
#include "GlobalVariables.h"
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

std::string CodeGenerator::globalVariablesDotHCode =
    R"(#ifndef __GLOBAL_VARIABLES_H
#define __GLOBAL_VARIABLES_H

#include "Sprite.h"

class GlobalVariables {
public:
${SPRITES}
  // you can also add your own variables here
};

#endif // __GLOBAL_VERIABLES_H
)";

std::string CodeGenerator::globalVariablesDotCppCode =
    R"(
#include "GlobalVariables.h"

${SPRITES}

)";

std::string CodeGenerator::gameHandlerDotHCode =
    R"(#ifndef __GAME_HANDLER_H
#define __GAME_HANDLER_H

#include <algorithm>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "SDL2/SDL.h"

#include "BaseObjectClass.h"
#include "monitor/RuiMonitor.h"
#include "utils/Rect.h"

${INCLUDE_ALL_OBJECTS}

class GameHandler {
public:
  GameHandler(RuiMonitor &monitor) : monitor(monitor) {}

  void start();

  // Arguments:
  // - room name
  // - the object itself
  void addObject(const std::string &, std::shared_ptr<BaseObjectClass>);

  // Arguments:
  // - name of the room to check in
  // - an object instance
  // - list of object names
  // - new position of the instance
  // Return value:
  // - the collided object is any collisions happens, otherwise nullptr
  std::shared_ptr<BaseObjectClass> isCollide(const std::string &, const BaseObjectClass &,
                                             const std::vector<std::string> &, std::pair<double, double>);

  // Arguments:
  // - name of the room to check in
  // - an object instance
  // - another object instance
  // - new position for the first object instance
  // Return value:
  // - whether they'll collide or not
  bool isCollide(const std::string &, const BaseObjectClass &, const BaseObjectClass &, std::pair<double, double>);

${CREATE_FUNCTIONS_FOR_ALL_OBJECTS}

  const std::string &getCurrentRoom() const { return this->currentRoom; }
  void setCurrentRoom(const std::string &);

private:
  using allObjectsContainer = std::map<std::string, std::vector<std::shared_ptr<BaseObjectClass>>>;
  using eventFunction = std::function<void(std::shared_ptr<BaseObjectClass> &, GameHandler &)>;

  void render(std::shared_ptr<BaseObjectClass>);

  // arguments:
  // - isApplicable function
  // - function to be run if is applicable (one of the object's class members - checkout GameHandler::eventFunctions)
  // - function to be run after the event function is run
  // - function to be run after the event function is run
  void runFunctionWithAllObjects(std::function<bool(std::shared_ptr<BaseObjectClass>)>, eventFunction,
                                 std::function<void(std::shared_ptr<BaseObjectClass>)>,
                                 std::function<void(std::shared_ptr<BaseObjectClass>)>);

  void handleMouseUpDownEvents(SDL_Event);
  void handleMouseMoveEvents(SDL_Event);
  void handleKeyUpDownEvents(SDL_Event);

  // room name -> its objects
  allObjectsContainer gameObjects;

  RuiMonitor monitor;
  std::string currentRoom = "";

  static const std::map<std::string, eventFunction> eventFunctions;
};

#endif // __GAME_HANDLER_H
)";

std::string CodeGenerator::gameHandlerDotCppCode =
    R"(
#include "GameHandler.h"

const std::map<std::string, GameHandler::eventFunction> GameHandler::eventFunctions = {
    {"createEvent", &BaseObjectClass::createEvent},
    {"destroyEvent", &BaseObjectClass::destroyEvent},
    {"stepEvent", &BaseObjectClass::stepEvent},
    {"leftMouseDown", &BaseObjectClass::leftMouseDown},
    {"rightMouseDown", &BaseObjectClass::rightMouseDown},
    {"middleMouseDown", &BaseObjectClass::middleMouseDown},
    {"leftMouseUp", &BaseObjectClass::leftMouseUp},
    {"rightMouseUp", &BaseObjectClass::rightMouseUp},
    {"middleMouseUp", &BaseObjectClass::middleMouseUp},
    {"mouseMove", &BaseObjectClass::mouseMove},
    {"mouseEnter", &BaseObjectClass::mouseEnter},
    {"mouseLeave", &BaseObjectClass::mouseLeave},
    {"keyDown", &BaseObjectClass::keyDown},
    {"keyUp", &BaseObjectClass::keyUp},
};

void GameHandler::start() {
  SDL_Event event;

  while (true) {
    if (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT)
        exit(0); // TODO maybe do something fancier?
      else if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP)
        this->handleMouseUpDownEvents(event);
      else if (event.type == SDL_MOUSEMOTION)
        this->handleMouseMoveEvents(event);
      else if ((event.type == SDL_KEYDOWN && event.key.repeat == 0) || event.type == SDL_KEYUP)
        this->handleKeyUpDownEvents(event);
    }

    auto trueFunction = [](std::shared_ptr<BaseObjectClass>) { return true; };
    auto emptyFunction = [](std::shared_ptr<BaseObjectClass>) {};
    this->runFunctionWithAllObjects(trueFunction, GameHandler::eventFunctions.at("stepEvent"), emptyFunction,
                                    emptyFunction);
    // handler create and destroy later

    // move the objects (if they have vx or vy)
    this->runFunctionWithAllObjects(
        trueFunction,
        [](std::shared_ptr<BaseObjectClass> object, GameHandler &) {
          object->setPosition({object->getPosition().first + object->getVelocity().first,
                               object->getPosition().second + object->getVelocity().second});
        },
        emptyFunction, emptyFunction);

    // handle collisions
    for (auto itr = gameObjects.begin(); itr != gameObjects.end(); itr++) {
      auto &objects = itr->second; // all objects in the current room
      for (int i = 0u; i < objects.size(); i++) {
        for (int j = i + 1; j < objects.size(); j++) {
          if (i == j)
            continue;

          Rect object1Rect = {
              objects.at(i)->getPosition().first,
              objects.at(i)->getPosition().second,
              double(objects.at(i)->sprite.getCurrentFrameSize().first),
              double(objects.at(i)->sprite.getCurrentFrameSize().second),
          };
          Rect object2Rect = {
              objects.at(j)->getPosition().first,
              objects.at(j)->getPosition().second,
              double(objects.at(j)->sprite.getCurrentFrameSize().first),
              double(objects.at(j)->sprite.getCurrentFrameSize().second),
          };
          if (Geometry::trimRect(object1Rect, object2Rect).second) {
            objects.at(i)->collidedWith(objects.at(j), *this);
            objects.at(j)->collidedWith(objects.at(i), *this);
          }
        }
      }
    }

    // check object deletes
    for (auto itr = gameObjects.begin(); itr != gameObjects.end(); itr++) {
      auto &objects = itr->second;
      for (auto objectItr = objects.begin(); objectItr != objects.end();) {
        auto object = *objectItr;
        if (object->toBeDestroyed) {
          objectItr->get()->destroyEvent(*this);
          objectItr = objects.erase(objectItr);
        }
        else
          objectItr++;
      }
    }

    monitor.clear({0, 0, 0, 255});
    if (currentRoom != "") {
      auto &objects = gameObjects.find(currentRoom)->second;
      for (auto &object : objects) {
        this->render(object);
      }
    }
    monitor.update();
  }
}

void GameHandler::addObject(const std::string &roomName, std::shared_ptr<BaseObjectClass> object) {
  if (!gameObjects.contains(roomName))
    gameObjects.insert({roomName, {}});
  gameObjects.find(roomName)->second.push_back(object);
  object->createEvent(*this);
}

void GameHandler::render(std::shared_ptr<BaseObjectClass> object) {
  auto currentFrame = object->sprite.getCurrentFrameAndProceed();
  auto windowSize = monitor.getMonitorSize();
  monitor.drawImage({object->getPosition().first, object->getPosition().second, -1, -1},
                    {0, 0, double(windowSize.first), double(windowSize.second)}, currentFrame->getTexture(),
                    currentFrame->getImagePath());
}

std::shared_ptr<BaseObjectClass> GameHandler::isCollide(const std::string &roomName, const BaseObjectClass &object,
                                                        const std::vector<std::string> &objectNames,
                                                        std::pair<double, double> newPosition) {
  for (auto itr : this->gameObjects)
    if (itr.first == roomName)
      for (auto &otherObject : itr.second)
        if (std::find(objectNames.begin(), objectNames.end(), otherObject->getObjectName()) != objectNames.end())
          if (this->isCollide(roomName, object, *otherObject, newPosition))
            return otherObject;
  return nullptr;
}

bool GameHandler::isCollide(const std::string &roomName, const BaseObjectClass &object,
                            const BaseObjectClass &otherObject, std::pair<double, double> newPosition) {
  Rect objectRect = {
      newPosition.first,
      newPosition.second,
      double(object.sprite.getCurrentFrame()->getSize().first),
      double(object.sprite.getCurrentFrame()->getSize().second),
  };

  Rect otherObjectRect = {
      otherObject.getPosition().first,
      otherObject.getPosition().second,
      double(otherObject.sprite.getCurrentFrame()->getSize().first),
      double(otherObject.sprite.getCurrentFrame()->getSize().second),
  };

  return Geometry::trimRect(objectRect, otherObjectRect).second;
}

${CREATE_OBJECT_FUNCTIONS_IMPLEMENTATIONS}

void GameHandler::setCurrentRoom(const std::string &roomName) { this->currentRoom = roomName; }

void GameHandler::runFunctionWithAllObjects(std::function<bool(std::shared_ptr<BaseObjectClass>)> isApplicable,
                                            eventFunction function,
                                            std::function<void(std::shared_ptr<BaseObjectClass>)> beforeEvent,
                                            std::function<void(std::shared_ptr<BaseObjectClass>)> afterEvent) {
  for (auto itr = gameObjects.begin(); itr != gameObjects.end(); itr++) {
    auto objects = itr->second;
    for (auto object : objects) {
      if (isApplicable(object)) {
        beforeEvent(object);
        function(object, *this);
        afterEvent(object);
      }
    }
  }
}

void GameHandler::handleMouseUpDownEvents(SDL_Event event) {
  auto x = event.button.x, y = event.button.y;
  auto isApplicable = [x, y](std::shared_ptr<BaseObjectClass> object) {
    auto objectImageSize = object->sprite.getCurrentFrameSize();
    return Geometry::isPointInsideRect(double(x), double(y),
                                       {object->getPosition().first, object->getPosition().second,
                                        double(objectImageSize.first), double(objectImageSize.second)});
  };

  auto emptyFunction = [](std::shared_ptr<BaseObjectClass>) {};
  if (event.type == SDL_MOUSEBUTTONDOWN) {
    if (event.button.button == SDL_BUTTON_LEFT)
      this->runFunctionWithAllObjects(isApplicable, GameHandler::eventFunctions.at("leftMouseDown"), emptyFunction,
                                      emptyFunction);
    else if (event.button.button == SDL_BUTTON_MIDDLE)
      this->runFunctionWithAllObjects(isApplicable, GameHandler::eventFunctions.at("middleMouseDown"), emptyFunction,
                                      emptyFunction);
    else if (event.button.button == SDL_BUTTON_RIGHT)
      this->runFunctionWithAllObjects(isApplicable, GameHandler::eventFunctions.at("rightMouseDown"), emptyFunction,
                                      emptyFunction);
  } else if (event.type == SDL_MOUSEBUTTONUP) {
    if (event.button.button == SDL_BUTTON_LEFT)
      this->runFunctionWithAllObjects(isApplicable, GameHandler::eventFunctions.at("leftMouseUp"), emptyFunction,
                                      emptyFunction);
    else if (event.button.button == SDL_BUTTON_MIDDLE)
      this->runFunctionWithAllObjects(isApplicable, GameHandler::eventFunctions.at("middleMouseUp"), emptyFunction,
                                      emptyFunction);
    else if (event.button.button == SDL_BUTTON_RIGHT)
      this->runFunctionWithAllObjects(isApplicable, GameHandler::eventFunctions.at("rightMouseUp"), emptyFunction,
                                      emptyFunction);
  }
}

void GameHandler::handleMouseMoveEvents(SDL_Event event) {
  auto emptyFunction = [](std::shared_ptr<BaseObjectClass>) {};

  auto x = event.motion.x, y = event.motion.y;
  this->runFunctionWithAllObjects([](std::shared_ptr<BaseObjectClass>) { return true; },
                                  GameHandler::eventFunctions.at("mouseMove"), emptyFunction, emptyFunction);

  auto revertIsMouseInside = [](std::shared_ptr<BaseObjectClass> object) {
    object->isMouseInside = !object->isMouseInside;
  };

  auto mouseEnterIsApplicable = [x, y](std::shared_ptr<BaseObjectClass> object) {
    auto objectSize = object->sprite.getCurrentFrameSize();
    return !object->isMouseInside &&
           Geometry::isPointInsideRect(double(x), double(y),
                                       {object->getPosition().first, object->getPosition().second,
                                        double(objectSize.first), double(objectSize.second)});
  };

  this->runFunctionWithAllObjects(mouseEnterIsApplicable, GameHandler::eventFunctions.at("mouseEnter"), emptyFunction,
                                  revertIsMouseInside);

  auto mouseLeaveIsApplicable = [x, y](std::shared_ptr<BaseObjectClass> object) {
    auto objectSize = object->sprite.getCurrentFrameSize();
    return object->isMouseInside &&
           !Geometry::isPointInsideRect(double(x), double(y),
                                        {object->getPosition().first, object->getPosition().second,
                                         double(objectSize.first), double(objectSize.second)});
  };

  this->runFunctionWithAllObjects(mouseLeaveIsApplicable, GameHandler::eventFunctions.at("mouseLeave"), emptyFunction,
                                  revertIsMouseInside);
}

void GameHandler::handleKeyUpDownEvents(SDL_Event event) {
  auto key = event.key.keysym.sym;
  auto setKeyInObjectFunction = [key](std::shared_ptr<BaseObjectClass> object) { object->keyboardEventValue = key; };

  auto emptyFunction = [](std::shared_ptr<BaseObjectClass>) {};
  auto trueFunction = [](std::shared_ptr<BaseObjectClass>) { return true; };

  if (event.type == SDL_KEYDOWN)
    this->runFunctionWithAllObjects(trueFunction, GameHandler::eventFunctions.at("keyDown"), setKeyInObjectFunction,
                                    emptyFunction);
  else if (event.type == SDL_KEYUP)
    this->runFunctionWithAllObjects(trueFunction, GameHandler::eventFunctions.at("keyUp"), setKeyInObjectFunction,
                                    emptyFunction);
}
)";
