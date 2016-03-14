
CC=g++
CXXFLAGS=-std=c++11 -Wall -Wextra -Wshadow -Wunused -Wconversion -O2 -static

SRC=$(wildcard src/*.cpp)
OBJ=$(patsubst src/%.cpp,obj/%.o,$(SRC))

bin/main: $(OBJ)
	mkdir -p bin
	$(CC) $(CXXFLAGS) -o $@ $(OBJ)

$(OBJ): obj/%.o: src/%.cpp
	mkdir -p obj
	$(CC) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f obj/*.o
	rm -f bin/main
