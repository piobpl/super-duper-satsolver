
CC=g++
CXXFLAGS=-std=c++11 -Wall -Wextra -Wshadow -Wunused -Wconversion -O2 -static

ALLSOURCES=$(wildcard src/*.cpp)

MAINSOURCES=$(wildcard src/*_main.cpp)
TESTSOURCES=$(wildcard src/*_test.cpp)

TARGETS=$(patsubst src/%_main.cpp,%,$(MAINSOURCES))
SOURCES=$(filter-out $(MAINSOURCES) $(TESTSOURCES),$(ALLSOURCES))
OBJECTS=$(patsubst src/%.cpp,obj/%.o,$(SOURCES))
TESTS=$(patsubst src/%.cpp,obj/%.o,$(TESTSOURCES))

$(TARGETS): % : src/%_main.cpp $(OBJECTS)
	@mkdir -p bin
	$(CC) $(CXXFLAGS) -o bin/$@ $^

$(OBJECTS) $(TESTS): obj/%.o : src/%.cpp
	mkdir -p obj
	$(CC) $(CXXFLAGS) -c $< -o $@

tester:	$(OBJECTS) $(TESTS)
	$(CC) $(CXXFLAGS) -o bin/$@ $^ -lgtest -lgtest_main -pthread

lint:
	./cpplint.py src/*

test: tester
	./bin/tester

clean:
	rm -f $(OBJECTS) $(TESTS)
	rm -f $(patsubst %,bin/%,$(TARGETS) tester)

debugmake:
	@echo "ALLSOURCES:" $(ALLSOURCES)
	@echo "MAINSOURCES:" $(MAINSOURCES)
	@echo "TESTSOURCES:" $(TESTSOURCES)
	@echo "MAINSOURCES + TESTSOURCES:" $(MAINSOURCES) $(TESTSOURCES)
	@echo "TARGETS:" $(TARGETS)
	@echo "SOURCES:" $(SOURCES)
	@echo "OBJECTS:" $(OBJECTS)
	@echo "TESTS:" $(TESTS)

.PHONY: lint test clean debugmake
