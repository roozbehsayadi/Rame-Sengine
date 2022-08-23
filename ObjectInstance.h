#ifndef __OBJECT_INSTANCE_H
#define __OBJECT_INSTANCE_H

#include "Object.h"
#include "RUI/widgets/elements/ScreenObject.h"

class ObjectInstace : public Object {
public:
  ObjectInstace(const std::string &objectName, const std::string &instanceName, Sprite &sprite,
                std::shared_ptr<ScreenObject> screenObject)
      : Object(objectName, sprite), screenObject(screenObject), instanceName(instanceName) {}

  const std::string &getInstanceName() const { return this->instanceName; }

  int getY() const { return screenObject->getPositionPixel().y; }
  int getX() const { return screenObject->getPositionPixel().x; }

protected:
  std::shared_ptr<ScreenObject> screenObject;

  std::string instanceName;
};

#endif // __OBJECT_INSTANCE_H
