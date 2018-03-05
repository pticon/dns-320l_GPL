#! /bin/sh

mkdir _install
./configure --build=i386-linux --host=arm-mv5sft-linux-gnueabi --prefix=$PWD/_install CFLAGS=-DUSE_LINKER_FINI_OPTION --cache-file=linux-cache
make
make install
