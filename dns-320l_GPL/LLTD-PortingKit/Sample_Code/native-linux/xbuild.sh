#!/bin/sh

unset CFLAGS
unset LDFLAGS
unset LIBS

source ../../../xcp.sh

xbuild()
{
   make
}

xinstall()
{
   xcp lld2d ${ROOT_FS}/sbin
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
