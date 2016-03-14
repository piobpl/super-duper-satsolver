#include <algorithm>
#include <deque>
#include <iostream>
#include <vector>

#include "clause.h"
#include "input.h"
#include "random_solver.h"

int n;
std::vector<Clause> clauses;

int main(){
	Input::read(n, clauses);

	RandomSolver solver(10000);

	do{
		solver.solve(n, clauses);
	}while(!solver.success());

	std::cout << solver.solution() << std::endl;
}
