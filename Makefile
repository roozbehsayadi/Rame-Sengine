.PHONY: all run clean lines

all: build/main.o
	cd RUI && make && cp -a build/. ../build/ && cd ..
	g++ -Ibuild build/*.o -o build/GameSengine.out -Wall -g -O2 -std=c++2a -lSDL2 -lSDL2_ttf -lSDL2_image
run:
	./build/GameSengine.out
build/main.o: main.cpp
	g++ -std=c++2a -c -IRUI -o build/main.o main.cpp
clean:
	rm -rf build/*.o build/*.gch build/*.out
lines:
	find . -type f -name "*.cpp" -o -name "*.h" -o -name "Makefile" | xargs wc -l
