#!/bin/sh

tsmon:
	rm -rf dist
	mkdir dist || exit 1
	(cd dist && cmake .. && cmake --build . -j 5)

clean:
	rm -rf dist
