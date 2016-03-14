#include <algorithm>
#include <deque>
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

	printf("SATISFIABLE\n");
	for(int i = 1; i <= n; ++i)
		printf("%d ", solver.solution()[i] ? 1 : 0);
	printf("\n");
}
