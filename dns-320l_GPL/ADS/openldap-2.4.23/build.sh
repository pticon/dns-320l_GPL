#! /bin/sh

mkdir _install
./configure --build=i386-linux --host=arm-mv5sft-linux-gnueabi --prefix=$PWD/_install --with-yielding-select=manual --without-cyrus-sasl --enable-bdb
cp ./include/portable.bk ./include/portable.h
make

