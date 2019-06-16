#!/usr/bin/env bash

set -e

export TRAVIS_COMPILER=g++-7
export CXX=g++-7
export CXX_FOR_BUILD=g++-7
export CC=gcc-7
export CC_FOR_BUILD=gcc-7

export NUM_JOBS=1

sudo apt-get install -yq curl

mkdir build
cd build
build-wrapper-linux-x86-64 --out-dir ../bw-output cmake .. -DCMAKE_BUILD_TYPE=Debug
make

cd ..
sonar-scanner