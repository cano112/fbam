#!/bin/sh

version_string=$(cat version.txt)

echo "Tagging release with version: $version_string"
echo -e "machine github.com\n  login $GH_TOKEN" > ~/.netrc
git config --local user.name "Travis CI"
git config --global user.email "travis@travis-ci.org"
git tag "$version_string"
git push --tags
