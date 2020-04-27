#!/bin/sh

version_string=$(cat version.txt)

git config --local user.name "Travis"
git tag "$version_string"
git push --tags
