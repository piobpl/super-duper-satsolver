#ifndef __SOLVER_H
#define __SOLVER_H

#include <vector>
#include "model.h"

class Solver {
public:
	virtual void solve(int n, std::vector<Clause> clauses) = 0;
	virtual bool success() = 0;
	virtual Model solution() = 0;
};

#endif
