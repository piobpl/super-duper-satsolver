#ifndef __INPUT_H
#define __INPUT_H

#include <cassert>
#include <iostream>
#include <vector>

#include "clause.h"

class Input {
public:
	static void read(int &n, std::vector<Clause> &clauses) {
		int m;
		char buff[4096];
		do{
			assert(scanf("%[^\n] ", buff) == 1);
		}while(buff[0] == 'c');
		assert(sscanf(buff, "p cnf %d %d", &n, &m) == 2);
		for(int i = 0; i < m; ++i){
			Clause clause;
			for(;;){
				int v;
				assert(scanf("%d", &v) == 1);
				if(v == 0) break;
				clause.add(v);
			}
			if(!clause.trivial())
				clauses.push_back(clause);
		}
		m = (int)clauses.size();
		std::cerr << "Got " << n << " variables and "
			<< m << " non-trivial clauses\n";
	}
};

#endif
