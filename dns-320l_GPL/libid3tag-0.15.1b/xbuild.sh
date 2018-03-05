#!/bin/sh

unset CFLAGS
unset LDFLAGS
unset LIBS

source ../xcp.sh

xbuild()
{
	if [ ! -e ../zlib-1.2.3/libz.so ]; then
		echo make zlib first!!!
		return
	fi
	
	make clean ; make distclean
	./configure CFLAGS=-I../zlib-1.2.3/ LDFLAGS=-L../zlib-1.2.3/ --host=${TARGET_HOST}
	make
}

xinstall()
{
	${CROSS_COMPILE}strip -s .libs/libid3tag.so.0
	xcp .libs/libid3tag.so.0 ${XLIB_DIR}
	xcp .libs/libid3tag.so.0 ${ROOT_FS}/lib
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
