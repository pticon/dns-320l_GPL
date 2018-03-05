#!/bin/sh

unset CFLAGS
unset LDFLAGS
unset LIBS

source ../xcp.sh

xbuild()
{
   make clean
   ./configure --shared --prefix=$(readlink -f $PWD/../_xinstall/${PROJECT_NAME})
   make
   make install
}

xinstall()
{
   ${CROSS_COMPILE}strip -s libz.so.1
   ${CROSS_COMPILE}strip -s libz.so
   
   xcp libz.so.1 ${ROOT_FS}/lib
   xcp libz.so ${XLIB_DIR}
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
