#! /bin/sh

mkdir _install
./configure --build=i386-linux --host=arm-marvell-linux-gnueabi --prefix=$PWD/_install
make
make install
