INSTALLROOT=$(HOME)

.PHONY: build test clean debug release

all: build

debug:
	mkdir -p build
	cd build && cmake -DCMAKE_BUILD_TYPE=Debug ..

release:
	mkdir -p build
	cd build && cmake -DCMAKE_BUILD_TYPE=Release ..

build:
	cd build && cmake --build . -j 5

install: build
	for i in muri mb64 sha1 bins flink; do cp build/$$i ${INSTALLROOT}/bin; done

test: build
	cd build/t && ctest

clean:
	rm -rf build

help:
	@echo "=============================================="
	@echo "Targets:"
	@echo "    all"
	@echo "        Alias for build"
	@echo "    debug"
	@echo "        Debug configuration (debug or release needed to build)"
	@echo "    release"
	@echo "        Release configuration (debug or release needed to build)"
	@echo "    build"
	@echo "        Build code, preferably incrementally, from build directory"
	@echo "    install"
	@echo "        Install custom binaries in ${HOME}/bin"
	@echo "    test"
	@echo "        Runs build and then testcases with ctest"
	@echo "    clean"
	@echo "        Delete all build output"
	@echo "    help"
	@echo "        This output"
