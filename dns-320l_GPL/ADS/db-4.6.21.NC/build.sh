#! /bin/sh

cd os
mkdir _install
../dist/configure --build=i386-linux --host=arm-mv5sft-linux-gnueabi --prefix=$PWD/_install
make
make install
