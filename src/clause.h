#ifndef __CLAUSE_H
#define __CLAUSE_H

#include <bitset>
#include <iostream>

const int VARCOUNT = 250;
const int BLOCKS = (2*VARCOUNT + 63) / 64;

typedef unsigned long long block_type;

int var(int v){
	if(v > 0)
		return 2*v - 2;
	return -2*v - 1;
}

class Clause {
public:
	static int MAX_VARIABLES() {
		return VARCOUNT;
	}
	block_type data[BLOCKS];
	Clause(){
		for(int i = 0; i < BLOCKS; ++i)
			data[i] = 0;
	}
	Clause(const Clause &o){
		for(int i = 0; i < BLOCKS; ++i)
			data[i] = o.data[i];
	}
	bool unsatisfiable() const {
		for(int i = 0; i < BLOCKS; ++i)
			if(data[i])
				return 0;
		return 1;
	}
	bool trivial() const {
		for(int i = 0; i < BLOCKS; ++i)
			if(data[i] & (data[i] >> 1))
				return 1;
		return 0;
	}
	bool has(int v) const {
		v = var(v);
		return data[v / 64] & (1ULL << (v % 64));
	}
	void add(int v) {
		v = var(v);
		data[v / 64] |= (1ULL << (v % 64));
	}
	void remove(int v) {
		v = var(v);
		data[v / 64] &= ~(1ULL << (v % 64));
	}
};

bool operator<(const Clause &a, const Clause &b){
	for(int i = 0; i < BLOCKS; ++i)
		if(a.data[i] != b.data[i])
			return a.data[i] < b.data[i];
	return 0;
}

bool operator==(const Clause &a, const Clause &b){
	for(int i = 0; i < BLOCKS; ++i)
		if(a.data[i] != b.data[i])
			return 0;
	return 1;
}

Clause operator|(const Clause &a, const Clause &b){
	Clause c;
	for(int i = 0; i < BLOCKS; ++i)
		c.data[i] = (a.data[i] | b.data[i]);
	return c;
}

Clause resolve(const Clause &a, const Clause &b, int v){
	v = var(v);
	Clause c = (a | b);
	c.remove(v);
	c.remove(v^1);
	return c;
}

std::ostream& operator<<(std::ostream &out, const Clause &c){
	bool st = 1;
	out << "(";
	for(int i = 1; i < VARCOUNT; ++i){
		if(c.has(i)){
			if(!st) out << " v ";
			st = 0;
			out << i;
		}
		if(c.has(-i)){
			if(!st) out << " v ";
			st = 0;
			out << "~" << i;
		}
	}
	out << ")";
	return out;
}

#endif
