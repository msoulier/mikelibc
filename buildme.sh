#!/bin/sh

rm -rf build
mkdir build || exit 1
(cd build && cmake .. && make)
