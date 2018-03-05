#!/bin/sh

rm -rf autom4te.cache/
make clean 2>/dev/null

autoreconf -v -f -i
./configure --host=arm-gnu-linux CFLAGS="-O2 -I${XINC_DIR}" LDFLAGS="-L${XLIB_DIR} -lxmldbc"

make

$STRIP -s ez-ipupdate
