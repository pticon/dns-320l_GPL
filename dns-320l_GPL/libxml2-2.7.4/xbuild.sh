#!/bin/bash

unset CFLAGS
unset LDFLAGS
unset LIBS

source ../xcp.sh

xbuild()
{
   make clean
   ./configure --host=${TARGET_HOST} --with-python=no --prefix=${PWD}/../_xinstall/${PROJECT_NAME}
   make
   make install
}

xinstall()
{
   ${CROSS_COMPILE}strip -s .libs/libxml2.so.2.7.4
   
   xcp .libs/libxml2.so.2.7.4 ${ROOT_FS}/lib
   xcp .libs/libxml2.so ${XLIB_DIR}
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

