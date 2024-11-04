.PHONY: build test clean debug release

all: build

debug:
	mkdir build && cd build && cmake -DCMAKE_BUILD_TYPE=Debug ..

release:
	cd build && cmake -DCMAKE_BUILD_TYPE=Release ..

build:
	cd build && cmake --build . -j 5

install: build
	cp build/muri build/mb64 build/sha1 ${HOME}/bin

test: build
	cd build/t && ctest

clean:
	rm -rf build
