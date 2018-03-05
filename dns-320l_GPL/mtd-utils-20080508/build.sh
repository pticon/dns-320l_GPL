#!/bin/sh
unset CFLAGS
unset LDFLAGS
unset LIBS

export CROSS=${CROSS_COMPILE}
echo $CROSS
make clean
make WITHOUT_XATTR=1
		