#!/bin/sh

enabled="1"
command="./testreader ./sample.txt"
logfile="log_$(echo "$command" | tr ./ "_" | cut -d' ' -f2).jsonl"
path_pattern="sample.txt"

echo "Enabled: $enabled"
echo "Command: $command"
echo "Log pattern: $path_pattern"

docker rm --force fbam_build_dummy
docker build --no-cache \
  --build-arg enabled="$enabled" \
  --build-arg command="$command" \
  --build-arg logfile="$logfile" \
  --build-arg path_pattern="$path_pattern" \
  -t build-fbam-alpine .
docker create -ti --name fbam_build_dummy build-fbam-alpine bash
mkdir -p ./test_results
docker cp "fbam_build_dummy:/fbam/build/$logfile" "./test_results/$logfile"
docker logs fbam_build_dummy
echo "------- TEST RESULTS ---------"
cat "./test_results/$logfile"
