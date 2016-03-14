
CC=g++
CXXFLAGS=-std=c++11 -Wall -Wextra -Wshadow -Wunused -Wconversion -O2 -static

TARGETS=$(patsubst src/%_main.cpp,%,$(wildcard src/*.cpp))
SOURCES=$(filter-out $(wildcard src/*_main.cpp),$(wildcard src/*.cpp))
OBJECTS=$(patsubst src/%.cpp,obj/%.o,$(SOURCES))

$(TARGETS): % : src/%_main.cpp $(OBJECTS) 
	./cpplint.py src/*
	@mkdir -p bin
	$(CC) $(CXXFLAGS) -o bin/$@ $<

$(OBJECTS): obj/%.o : src/%.cpp
	mkdir -p obj
	$(CC) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS)
	rm -f $(patsubst %,bin/%,$(TARGETS))
