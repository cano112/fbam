#!/bin/sh

touch /tmp/file_access.log
cd /decorator || exit
mkdir build
cd build || exit 
cmake ..
make

