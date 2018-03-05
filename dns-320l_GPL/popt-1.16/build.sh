#! /bin/sh

mkdir _install
./configure --build=i386-linux --host=arm-mv5sft-linux-gnueabi --prefix=$PWD/_install
make
make install