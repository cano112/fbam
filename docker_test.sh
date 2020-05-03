#!/bin/sh

version_string=$(cat /fbam/version.txt)

mkdir /fbam/build
cd /fbam/build || exit 
cmake ..
make

touch "$HF_VAR_FS_MONIT_PATH_FILTER"
touch "$HF_VAR_FS_MONIT_LOGFILE"
echo "Ala ma kota" > "$HF_VAR_FS_MONIT_PATH_FILTER"
LD_PRELOAD=/fbam/build/libblockaccess.so.$version_string $HF_VAR_FS_MONIT_COMMAND
