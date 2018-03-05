#!/bin/sh
./configure --host=arm-linux --prefix=${PWD}/.install/ --enable-mount=no --enable-static=no --enable-shared=yes --disable-gss --disable-nfsv4 --disable-uuid
make
