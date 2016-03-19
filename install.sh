#!/bin/bash

# cpplint
wget -c "https://raw.githubusercontent.com/google/styleguide/gh-pages/cpplint/cpplint.py"
chmod +x cpplint.py

# gtest
sudo apt-get install libgtest-dev
sudo apt-get install cmake
cd /usr/src/gtest
sudo cmake CMakeLists.txt
sudo make
sudo cp *.a /usr/lib

# boost
sudo apt-get install libboost-all-dev
