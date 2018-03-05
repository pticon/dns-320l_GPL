#!/bin/sh
make clean ; make distclean
./configure --host=arm-gnu-linux CFLAGS="-I../../../DNS-345/module/include" LDFLAGS="-L../../../DNS-345/module/lib -lxmldbc"
make

$STRIP ez-ipupdate