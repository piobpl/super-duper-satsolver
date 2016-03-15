#!/bin/bash

mkdir -p tests
cd tests

#from http://www.cs.ubc.ca/~hoos/SATLIB/benchm.html

wget -c "http://www.cs.ubc.ca/~hoos/SATLIB/Benchmarks/SAT/RND3SAT/uf20-91.tar.gz"
wget -c "http://www.cs.ubc.ca/~hoos/SATLIB/Benchmarks/SAT/RND3SAT/uf50-218.tar.gz"
wget -c "http://www.cs.ubc.ca/~hoos/SATLIB/Benchmarks/SAT/RND3SAT/uf75-325.tar.gz"
wget -c "http://www.cs.ubc.ca/~hoos/SATLIB/Benchmarks/SAT/RND3SAT/uf100-430.tar.gz"
wget -c "http://www.cs.ubc.ca/~hoos/SATLIB/Benchmarks/SAT/RND3SAT/uf125-538.tar.gz"
wget -c "http://www.cs.ubc.ca/~hoos/SATLIB/Benchmarks/SAT/RND3SAT/uf150-645.tar.gz"
wget -c "http://www.cs.ubc.ca/~hoos/SATLIB/Benchmarks/SAT/RND3SAT/uf175-753.tar.gz"
wget -c "http://www.cs.ubc.ca/~hoos/SATLIB/Benchmarks/SAT/RND3SAT/uf200-860.tar.gz"
wget -c "http://www.cs.ubc.ca/~hoos/SATLIB/Benchmarks/SAT/RND3SAT/uf225-960.tar.gz"
wget -c "http://www.cs.ubc.ca/~hoos/SATLIB/Benchmarks/SAT/RND3SAT/uf250-1065.tar.gz"

tar xzf uf20-91.tar.gz
tar xzf uf50-218.tar.gz
tar xzf uf75-325.tar.gz
tar xzf uf100-430.tar.gz
tar xzf uf125-538.tar.gz
tar xzf uf150-645.tar.gz
tar xzf uf175-753.tar.gz
tar xzf uf200-860.tar.gz
tar xzf uf225-960.tar.gz
tar xzf uf250-1065.tar.gz

mv ai/hoos/Shortcuts/*/* .
mv ai/hoos/Research/SAT/Formulae/*/* .
rm -r ai
