#!/bin/sh

version_string=$(cat /fbam/version.txt)

mkdir /fbam/build
cd /fbam/build || exit 
cmake ..
make

touch sample.txt
touch /fbam/file_access.log
echo "Ala ma kota" > sample.txt
LD_PRELOAD=/fbam/build/libblockaccess.so.$version_string ./testreader ./sample.txt
