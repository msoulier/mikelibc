#!/bin/sh

rm -rf dist
mkdir dist || exit 1
(cd dist && cmake .. && make)
