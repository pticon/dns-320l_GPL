#!/bin/sh

unset CFLAGS
unset LDFLAGS
unset LIBS

source ../xcp.sh

xbuild()
{
	./configure --disable-tcl --host=${TARGET_HOST}
	make
}

xinstall()
{
	${CROSS_COMPILE}strip .libs/libsqlite3.so.0
	xcp .libs/libsqlite3.so.0 ${ROOT_FS}/lib/libsqlite3.so.0
	xcp .libs/libsqlite3.so.0 ${XLIB_DIR}/libsqlite3.so
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
