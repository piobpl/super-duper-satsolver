#ifndef __SOLVER_H
#define __SOLVER_H

#include <vector>

class Solver {
public:
	virtual void solve(int n, std::vector<Clause> clauses) = 0;
	virtual bool success() = 0;
	virtual std::vector<bool> solution() = 0;
};

#endif
