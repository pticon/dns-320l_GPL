#!/bin/sh

unset CFLAGS
unset LDFLAGS
unset LIBS

source ../xcp.sh

xbuild()
{
   ./configure --build=i386-linux --host=${CC%-*}
   make
}

xinstall()
{
   ${CROSS_COMPILE}strip -s gunzip
   ${CROSS_COMPILE}strip -s gzip
   
   xcp gunzip ${ROOT_FS}/bin
   xcp gzip ${ROOT_FS}/bin
}

xclean()
{
   if [ -e Makefile ]; then
   	make clean
   else
   	echo "can not find Makefile"
   fi
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
