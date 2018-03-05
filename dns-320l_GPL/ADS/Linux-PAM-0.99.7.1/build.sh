#! /bin/sh

mkdir _install
./configure --build=i386-linux --host=arm-mv5sft-linux-gnueabi --prefix=$PWD/_install LIBS="-lfl" LDFLAGS="-L/opt_gccarm/arm-mv5sft-linux-gnueabi/arm-mv5sft-linux-gnueabi/sys-root/usr/lib"
make clean
make

