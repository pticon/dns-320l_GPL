#!/bin/sh

unset CFLAGS
unset LDFLAGS
unset LIBS

source ../xcp.sh

PREFIX=$(readlink -f $PWD/../_xinstall/${PROJECT_NAME})

xbuild(){		
	./configure --host=arm-gnu-linux --prefix=$PREFIX CPPFLAGS=-I$PREFIX/include LDFLAGS=-L$PREFIX/lib
	make clean;make;make install
}	

xinstall(){	
	$STRIP  ./.libs/libpng.so.3
	$STRIP  ./.libs/libpng12.so.0
	xcp ./.libs/libpng.so.3 ${ROOT_FS}/lib/libpng.so.3
	xcp ./.libs/libpng12.so.0 ${ROOT_FS}/lib/libpng12.so.0
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
