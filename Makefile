
CC=g++
CXXFLAGS=-std=c++11 -Wall -Wextra -Wshadow -Wunused -Wconversion -O2 -static
#CXXFLAGS=-Wall -Wextra -pedantic -std=c++11 -O0 -Wshadow -Wformat=2 -Wfloat-equal -Wconversion -Wlogical-op -Wcast-qual -Wcast-align -D_FORTIFY_SOURCE=2 -fsanitize=address -fstack-protector
LDFLAGS=

ALLSOURCES=$(wildcard src/*.cpp)

MAINSOURCES=$(wildcard src/*_main.cpp)
TESTSOURCES=$(wildcard src/*_test.cpp)

TARGETS=$(filter-out solution, $(patsubst src/%_main.cpp,%,$(MAINSOURCES)))
SOURCES=$(filter-out $(MAINSOURCES) $(TESTSOURCES),$(ALLSOURCES))
OBJECTS=$(patsubst src/%.cpp,obj/%.o,$(SOURCES))
TESTS=$(patsubst src/%.cpp,obj/%.o,$(TESTSOURCES))

solution: src/solution_main.cpp $(SOURCES)
	$(CC) -std=c++11 -O2 -static -DNDEBUG -o solution $^ $(LDFLAGS)

zip:
	rm -f submit.zip
	zip submit.zip src/solution_main.cpp src/*.h $(SOURCES) Makefile

$(TARGETS): % : src/%_main.cpp $(OBJECTS)
	@mkdir -p bin
	$(CC) $(CXXFLAGS) -o bin/$@ $^ $(LDFLAGS)

$(OBJECTS) $(TESTS): obj/%.o : src/%.cpp
	mkdir -p obj
	$(CC) $(CXXFLAGS) -c $< -o $@

tester:	$(OBJECTS) $(TESTS)
	$(CC) $(CXXFLAGS) -o bin/$@ $^ $(LDFLAGS) -lgtest -lgtest_main -pthread

lint:
	./scripts/cpplint.py src/*

test: tester
	./bin/tester

do_coverage: CXXFLAGS=-std=c++11 -Wall -Wextra -Wshadow -Wunused -Wconversion \
	    -O0 -fprofile-arcs -ftest-coverage -static 
do_coverage: LDFLAGS=-lgcov
do_coverage: clean test
	mkdir -p coverage
	rm -f coverage/*
	./scripts/gcovr.py -r . -e ".+test\.cpp" --object-directory=./obj --exclude-unreachable-branches \
	    --html --html-details -o coverage/report.html
	@echo "Coverage statistics created in coverage/ dir"

coverage:
	-make do_coverage
	make clean

do_load: CXXFLAGS=-std=c++11 -Wall -Wextra -Wshadow -Wunused -Wconversion \
	    -O0 -fprofile-arcs -ftest-coverage -static 
do_load: LDFLAGS=-lgcov
do_load: clean solver
	./bin/solver < tests/uf200-015.cnf
	mkdir -p load
	rm -f load/*
	./scripts/gcovr.py -r . -e ".+test\.cpp" --object-directory=./obj --exclude-unreachable-branches \
	    --html --html-details -o load/report.html
	rm -f solver_main.gcda solver_main.gcno
	@echo "Load statistics created in load/ dir"

load:
	-make do_load
	make clean

benchmark: solver verifier
	./scripts/benchmark.py bin/solver bin/verifier tests/ --test-pattern uf200-.* --time-limit 5

clean:
	rm -f solution submit.zip
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

.PHONY: zip lint test coverage do_coverage load do_load benchmark clean debugmake all
