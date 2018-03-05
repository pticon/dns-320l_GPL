#!/bin/sh

unset CFLAGS
unset LDFLAGS
unset LIBS

source ../xcp.sh

xbuild()
{
   make clean
   make
}

xinstall()
{
   ${CROSS_COMPILE}strip -s libcgic.so

   xcp libcgic.so ${ROOT_FS}/lib
   xcp libcgic.so ${XLIB_DIR}
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


