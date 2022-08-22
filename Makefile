.PHONY: all run clean lines

all: build/main.o build/Sprite.o build/Object.o build/Room.o build/IdGenerator.o build/CodeGenerator.o
	cd RUI && make && cp -a build/. ../build/ && cd ..
	g++ -Ibuild build/*.o -o build/GameSengine.out -Wall -g -O2 -std=c++2a -lSDL2 -lSDL2_ttf -lSDL2_image
run:
	gdb -q build/GameSengine.out
build/main.o: main.cpp
	g++ -std=c++2a -c -IRUI -o build/main.o main.cpp
build/Sprite.o: Sprite.h Sprite.cpp
	g++ -std=c++2a -c -IRUI -o build/Sprite.o Sprite.cpp
build/Object.o: Object.h Object.cpp
	g++ -std=c++2a -c -IRUI -o build/Object.o Object.cpp
build/Room.o: Room.h Room.cpp
	g++ -std=c++2a -c -IRUI -o build/Room.o Room.cpp
build/IdGenerator.o: IdGenerator.h IdGenerator.cpp
	g++ -std=c++2a -c -IRUI -o build/IdGenerator.o IdGenerator.cpp
build/CodeGenerator.o: CodeGenerator.h CodeGenerator.cpp
	g++ -std=c++2a -c -IRUI -o build/CodeGenerator.o CodeGenerator.cpp
clean:
	rm -rf build/*.o build/*.gch build/*.out
lines:
	find . -type f -name "*.cpp" -o -name "*.h" -o -name "Makefile" | xargs wc -l
