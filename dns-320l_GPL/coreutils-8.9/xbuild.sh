#!/bin/sh

unset CFLAGS
unset LDFLAGS
unset LIBS

source ../xcp.sh

xbuild()
{
	[ -f Makefile ] && make clean
	./configure --host=${CC%-*}
	sed -i -e 's/doc man po tests //g' Makefile
	make
}

xinstall()
{
   ${CROSS_COMPILE}strip -s src/du
   ${CROSS_COMPILE}strip -s src/cp
   
   xcp src/du ${ROOT_FS}/bin
   xcp src/cp ${ROOT_FS}/bin
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

