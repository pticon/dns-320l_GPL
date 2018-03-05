#!/bin/sh

unset CFLAGS
unset LDFLAGS
unset LIBS

source ../xcp.sh

PREFIX=$(readlink -f $PWD/../_xinstall/${PROJECT_NAME})

xbuild(){
	./configure --host=arm-gnu-linux --prefix=$PREFIX --enable-unicode --disable-gui --enable-optimize --enable-shared CPPFLAGS=-I$PREFIX/include
	make clean;make;make install
}

xinstall()
{	
	$STRIP ./lib/libwx_baseu_net-2.8.so.0.5.0
	$STRIP ./lib/libwx_baseu-2.8.so.0.5.0
	xcp ./lib/libwx_baseu_net-2.8.so.0.5.0 ${MODULE_DIR}/apkg/addons/${PROJECT_NAME}/aMule/lib/libwx_baseu_net-2.8.so.0.5.0
	xcp ./lib/libwx_baseu-2.8.so.0.5.0 ${MODULE_DIR}/apkg/addons/${PROJECT_NAME}/aMule/lib/libwx_baseu-2.8.so.0.5.0
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
