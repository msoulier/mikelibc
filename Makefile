#!/bin/sh

debug:
	rm -rf dist
	mkdir dist || exit 1
	(cd dist && cmake -DCMAKE_BUILD_TYPE=Debug .. && cmake --build . -j 5)

clean:
	rm -rf dist
