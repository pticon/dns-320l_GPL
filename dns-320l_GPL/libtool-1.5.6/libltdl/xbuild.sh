#!/bin/sh

unset CFLAGS
unset LDFLAGS
unset LIBS

source ../../xcp.sh

xbuild()
{
	make clean ; make distclean
	./configure LDFLAGS=-s --host=${TARGET_HOST}
	make
}

xinstall()
{
	${CROSS_COMPILE}strip -s .libs/libltdl.so.3
#	xcp .libs/libltdl.so.3 ${XLIB_DIR}
	xcp .libs/libltdl.so.3 ${ROOT_FS}/lib
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
