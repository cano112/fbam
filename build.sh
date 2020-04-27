#!/bin/sh

mkdir build
cd build || exit 
cmake ..
make
