#!/bin/sh

version_string=$(cat version.txt)

echo "Tagging release with version: $version_string"
git config --local user.name "Travis CI"
git config --global user.email "travis@travis-ci.org"
export TRAVIS_TAG=$version_string
git tag "$version_string"
