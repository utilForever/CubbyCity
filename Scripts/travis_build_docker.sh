#!/usr/bin/env bash

set -e

if [ $# -eq 0 ]
  then
    docker build -t utilforever/cubbycity .
else
    docker build -f $1 -t utilforever/cubbycity:$2 .
fi
docker run utilforever/cubbycity