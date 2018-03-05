#!/bin/sh

unset CFLAGS
unset LDFLAGS
unset LIBS

source ../xcp.sh

xbuild()
{
	./configure --host=${TARGET_HOST} 
	make
}

xinstall()
{
	${CROSS_COMPILE}strip .libs/libevent.so
	${CROSS_COMPILE}strip .libs/libevent-2.0.so.5

	xcp .libs/libevent.so ${XLIB_DIR}/libevent.so
	xcp .libs/libevent-2.0.so.5 ${ROOT_FS}/lib/libevent-2.0.so.5
}

xclean()
{
  make clean ; make distclean
}

if [ "$1" = "build" ]; then
   xbuild
elif [ "$1" = "install" ]; then
   xinstall
elif [ "$1" = "clean" ]; then
   xclean
else
   echo "Usage : xbuild.sh build or xbuild.sh install or xbuild.sh clean"
fi
