#ifndef __ID_GENERATOR_H
#define __ID_GENERATOR_H

#include <string>

class IdGenerator {
public:
  static std::string getId();

private:
  static int currentIndex;
};

#endif
