#!/bin/bash

cmake .

make

./bin/XylonEngine "$@"
