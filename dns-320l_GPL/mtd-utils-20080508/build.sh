#!/bin/sh
unset CFLAGS
unset LDFLAGS
unset LIBS

export CROSS=${CROSS_COMPILE}
echo $CROSS
make clean
make CROSS=${CROSS_COMPILE} WITHOUT_XATTR=1
	
