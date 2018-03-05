#!/bin/sh
cd build_unix
../dist/configure --host=arm-linux --prefix=$PWD/install --enable-pthread_api --enable-rpc
make clean
make
make install
