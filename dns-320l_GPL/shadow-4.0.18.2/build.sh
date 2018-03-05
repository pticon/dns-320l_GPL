#!/bin/sh
./configure --host=arm-linux --without-selinux
cp -f libtool_628x libtool
make clean;make

		