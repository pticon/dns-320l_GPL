#!/bin/sh

unset CFLAGS
unset LDFLAGS
unset LIBS

source ../../xcp.sh

xbuild()
{
   make clean
   ../configure --host=$TARGET_HOST
   make
}

xinstall()
{
   ${CROSS_COMPILE}strip -s libntfs-3g/.libs/libntfs-3g.so.75.0.0
   ${CROSS_COMPILE}strip -s src/.libs/ntfs-3g
   
   xcp libntfs-3g/.libs/libntfs-3g.so.75.0.0 ${ROOT_FS}/lib
   xcp src/.libs/ntfs-3g ${ROOT_FS}/sbin
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
