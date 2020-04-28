#!/bin/sh

version_string=$(cat version.txt)
log_file_name=file_access.log

docker rm --force fbam_build_dummy
docker build --no-cache -t build-fbam-alpine .
docker create -ti --name fbam_build_dummy build-fbam-alpine bash
mkdir -p ./test_results
docker cp fbam_build_dummy:/fbam/$log_file_name ./test_results/$log_file_name
echo "------- TEST RESULTS ---------"
cat ./test_results/$log_file_name
