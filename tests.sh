#!/bin/bash

mkdir -p tests
cd tests

#from http://www.cs.ubc.ca/~hoos/SATLIB/benchm.html

wget -c "http://www.cs.ubc.ca/%7Ehoos/SATLIB/Benchmarks/SAT/RND3SAT/uf20-91.tar.gz"
wget -c "http://www.cs.ubc.ca/%7Ehoos/SATLIB/Benchmarks/SAT/RND3SAT/uf50-218.tar.gz"
wget -c "http://www.cs.ubc.ca/%7Ehoos/SATLIB/Benchmarks/SAT/RND3SAT/uf75-325.tar.gz"
wget -c "http://www.cs.ubc.ca/%7Ehoos/SATLIB/Benchmarks/SAT/RND3SAT/uf100-430.tar.gz"
wget -c "http://www.cs.ubc.ca/%7Ehoos/SATLIB/Benchmarks/SAT/RND3SAT/uf125-538.tar.gz"

tar xzf uf20-91.tar.gz

tar xzf uf50-218.tar.gz

tar xzf uf75-325.tar.gz
mv ai/hoos/Shortcuts/UF75.325.100/* .
rm -r ai

tar xzf uf100-430.tar.gz

tar xzf uf125-538.tar.gz
mv ai/hoos/Research/SAT/Formulae/UF125.538.100/* .
rm -r ai
