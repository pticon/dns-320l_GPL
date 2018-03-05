#!/bin/sh

unset CFLAGS
unset LDFLAGS
unset LIBS

source ../xcp.sh

xbuild()
{
	make 
}

xinstall()
{
	${CROSS_COMPILE}strip ./build/lib/libs3.so.2
	xcp ./build/lib/libs3.so.2 ${ROOT_FS}/lib/libs3.so.2
	xcp ./build/lib/libs3.so.2 ${XLIB_DIR}/libs3.so
}

xclean()
{
  make clean
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
