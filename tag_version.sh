#!/bin/sh

version_string=$(cat version.txt)

echo "Tagging release with version: $version_string"
git config --local user.name "Travis"
git tag "$version_string"
git push --tags
