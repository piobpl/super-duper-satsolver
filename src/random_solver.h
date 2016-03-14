#ifndef __RANDOM_SOLVER_H
#define __RANDOM_SOLVER_H

#include <algorithm>
#include <deque>
#include <random>
#include <vector>

#include "clause.h"
#include "solver.h"

class RandomSolver : public Solver {
	std::random_device r;
	std::default_random_engine e;
	std::uniform_int_distribution<int> u;

	int n;
	std::vector<Clause> clauses;
	Model model;
	bool solved;

	int limit;
	std::deque<int> order;

	bool boolean(){
		return u(e);
	}
public:
	RandomSolver() : RandomSolver(-1) {}

	RandomSolver(int _limit) : e(r()), u(0, 1), limit(_limit) {}

	void solve(int _n, std::vector<Clause> _clauses) override {
		n = _n;
		clauses = _clauses;
		int m = (int)clauses.size();

		model.resize(n);
		solved = 0;

		for(int i = 1; i <= n; ++i)
			model.set(i, boolean());
		
		for(int i = 0; i < m; ++i)
			order.push_back(i);
		std::random_shuffle(order.begin(), order.end());

		int p = 0;
		int cnt = 0;
		while(p < m){
			++cnt;
			if(cnt == limit) return;

			int c = order[p];
			if(model.satisifed(clauses[c])){
				++p;
			}else{
				order.erase(order.begin() + p);
				order.push_front(c);
				p = 0;
				for(int i = 1; i <= n; ++i)
					if(clauses[c].has(i) || clauses[c].has(-i))
						model.set(i, boolean());
			}
		}
		solved = 1;
	}

	bool success() override {
		return solved;
	}

	Model solution() override {
		return model;
	}
};

#endif
