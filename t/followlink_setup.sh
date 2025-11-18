#!/bin/sh

pwd > ~/output

rm -f link target
touch target
ln -s ./target link

exit 0
