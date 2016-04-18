
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
	$(CC) $(CXXFLAGS) -o bin/$@ $^ -lgtest -lgcov -lgtest_main -pthread

lint:
	./scripts/cpplint.py src/*

test: tester
	./bin/tester

coverage: CXXFLAGS=-std=c++11 -Wall -Wextra -Wshadow -Wunused -Wconversion \
	    -O0 -fprofile-arcs -ftest-coverage -static 
coverage: clean test
	mkdir -p coverage
	rm -f coverage/*
	./scripts/gcovr.py -r . -e ".+test\.cpp" --object-directory=./obj --exclude-unreachable-branches \
	    --html --html-details -o coverage/report.html
	make clean
	@echo "Coverage statistics created in coverage/ dir"

benchmark: solver verifier
	./scripts/benchmark.py ./bin/solver uf20-

clean:
	rm -f obj/*
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

all: lint test $(TARGETS)

.PHONY: lint test coverage benchmark clean debugmake all
