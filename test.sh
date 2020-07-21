#!/bin/bash

sudo rm -rf build/
docker run --rm -v $PWD:/project -w /project gabrielmilan/esp-idf:3.2.4 make -j$(grep -c ^processor /proc/cpuinfo)
