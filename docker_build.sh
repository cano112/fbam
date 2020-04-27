#!/bin/sh

mkdir /fbam/build
cd /fbam/build || exit 
cmake ..
make

ls -al /fbam/build
