#!/bin/sh

version_string=$(cat /fbam/version.txt)

export HF_VAR_FS_MONIT_ENABLED="1"
export HF_VAR_FS_MONIT_COMMAND="./testreader ./sample.txt"
export HF_VAR_FS_MONIT_LOGFILE="/fbam/file_access.log"
export HF_VAR_FS_MONIT_PATH_FILTER="sample.txt"

mkdir /fbam/build
cd /fbam/build || exit 
cmake ..
make

touch $HF_VAR_FS_MONIT_PATH_FILTER
touch $HF_VAR_FS_MONIT_LOGFILE
echo "Ala ma kota" > HF_VAR_FS_MONIT_PATH_FILTER
LD_PRELOAD=/fbam/build/libblockaccess.so.$version_string $HF_VAR_FS_MONIT_COMMAND
