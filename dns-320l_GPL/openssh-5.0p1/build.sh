#! /bin/sh

./configure --host=arm-linux --prefix=${PWD}/.install LD=$(CC) --with-zlib=${PWD}/../zlib-1.2.3/_install/
make