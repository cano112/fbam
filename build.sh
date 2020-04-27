#!/bin/sh

version_string=$(cat version.txt)

docker rm --force fbam_build_dummy
docker build -t build-fbam-alpine .
docker create -ti --name fbam_build_dummy build-fbam-alpine bash
mkdir ./target
docker cp fbam_build_dummy:/fbam/build/libblockaccess.so.$version_string ./target/libblockaccess.so.$version_string
