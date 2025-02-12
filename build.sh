#!/bin/bash

rm -rf CMakeCache.txt CMakeFiles/

cmake .

make

./bin/XylonEngine "$@"
