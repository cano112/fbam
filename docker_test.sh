#!/bin/sh

version_string=$(cat /fbam/version.txt)

mkdir /fbam/build
cd /fbam/build || exit 
cmake ..
make

touch "sample.txt"
touch "$HF_VAR_FS_MONIT_LOGFILE"
echo "Ala ma kota" > "sample.txt"
LD_PRELOAD=/fbam/build/libblockaccess.so.$version_string $HF_VAR_FS_MONIT_COMMAND
