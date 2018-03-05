#!/bin/sh

unset CFLAGS
unset LDFLAGS
unset LIBS

source ../xcp.sh

xbuild()
{
   touch *
   make clean
   ./configure --host=${TARGET_HOST}
   make
}

xinstall()
{
   ${CROSS_COMPILE}strip -s lsmod
   ${CROSS_COMPILE}strip -s insmod
   ${CROSS_COMPILE}strip -s rmmod
   
   xcp lsmod ${ROOT_FS}/bin
   xcp insmod ${ROOT_FS}/bin
   xcp rmmod ${ROOT_FS}/bin
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
