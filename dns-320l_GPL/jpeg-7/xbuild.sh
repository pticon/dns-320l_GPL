#!/bin/bash

unset CFLAGS
unset LDFLAGS
unset LIBS

source ../xcp.sh

xbuild()
{
	make clean ; make distclean
	./configure LDFLAGS=-s --host=${TARGET_HOST} --prefix=${PWD}/../_xinstall/${PROJECT_NAME}
	make
}

xinstall()
{
	${CROSS_COMPILE}strip -s .libs/libjpeg.so.7
	xcp .libs/libjpeg.so.7 ${XLIB_DIR}/libjpeg.so
	xcp .libs/libjpeg.so.7 ${ROOT_FS}/lib
	
	${CROSS_COMPILE}strip -s .libs/djpeg
	xcp .libs/djpeg ${ROOT_FS}/bin
}

xclean()
{
	make clean
	make distclean
}

if [ "$1" = "build" ]; then
	xbuild
elif [ "$1" = "install" ]; then
	xinstall
elif [ "$1" = "clean" ]; then
	xclean
else
	echo "Usage : xbuild.sh {build | install | clean}"
fi
