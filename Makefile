#!/bin/sh

debug:
	rm -rf dist
	mkdir dist || exit 1
	(cd dist && cmake -DCMAKE_BUILD_TYPE=Debug .. && cmake --build . -j 5)

install:
	cp dist/muri dist/mb64 dist/sha1 ${HOME}/bin

clean:
	rm -rf dist
