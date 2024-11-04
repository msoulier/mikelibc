#!/bin/sh

debug:
	rm -rf build
	mkdir build || exit 1
	cd build && cmake -DCMAKE_BUILD_TYPE=Debug .. && cmake --build . -j 5

install:
	cp build/muri build/mb64 build/sha1 ${HOME}/bin

test:
	cd t && cmake . && cmake --build .
	cd build && ctest

clean:
	rm -rf build
