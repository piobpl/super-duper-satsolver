#!/bin/bash

# scripts
mkdir -p scripts
cd scripts

# scripts/benchmark
wget -c "http://piotrbejda.student.tcs.uj.edu.pl/benchmark.py"
chmod +x benchmark.py

# scripts/cpplint
wget -c "https://raw.githubusercontent.com/google/styleguide/gh-pages/cpplint/cpplint.py"
chmod +x cpplint.py

# scripts/gcovr
wget -c "https://pypi.python.org/packages/source/g/gcovr/gcovr-3.2.tar.gz#md5=afa74c6c80cfae96bb6771a431a57c31"
tar xzf gcovr-3.2.tar.gz gcovr-3.2/scripts/gcovr
mv gcovr-3.2/scripts/gcovr gcovr.py
rm -r gcovr-3.2
rm gcovr-3.2.tar.gz

# scripts end
cd ..

# gtest
sudo apt-get install libgtest-dev
sudo apt-get install cmake
cd /usr/src/gtest
sudo cmake CMakeLists.txt
sudo make
sudo cp *.a /usr/lib

# boost
sudo apt-get install libboost-all-dev
