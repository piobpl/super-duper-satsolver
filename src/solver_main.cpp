/* Copyright 2016 Authors */

#include <algorithm>
#include <cstdio>
#include <deque>
#include <iostream>
#include <vector>

#include "../src/clause.h"
#include "../src/input.h"
#include "../src/random_solver.h"

int n;
std::vector<Clause> clauses;

int main() {
    Input::read(stdin, &n, &clauses);

    RandomSolver solver(10000);

    do {
        solver.solve(n, clauses);
    }while(!solver.success());

    std::cout << solver.solution() << std::endl;
}
