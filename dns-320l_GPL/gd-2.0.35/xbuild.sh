#!/bin/bash


source ../xcp.sh

xbuild()
{
	make clean
	make distclean

	./configure \
	--build=i386-linux \
	--host=${TARGET_HOST} \
	--prefix=${PWD}/../_xinstall/${PROJECT_NAME} \
	--with-freetype=${PWD}/../_xinstall/${PROJECT_NAME} \
	--with-fontconfig=${PWD}/../_xinstall/${PROJECT_NAME}/include \
	CFLAGS="${CFLAGS} -I${PWD}/../_xinstall/${PROJECT_NAME}/include -I${PWD}/../_xinstall/${PROJECT_NAME}/include/freetype2" \
	LDFLAGS="${LDFLAGS} -L${PWD}/../_xinstall/${PROJECT_NAME}/lib -lxml2 -liconv -lz"

	make
	make install
}

xinstall()
{
   ${CROSS_COMPILE}strip -s .libs/libgd.so.2.0.0
   
   xcp .libs/libgd.so.2.0.0 ${ROOT_FS}/lib/libgd.so.2
   xcp .libs/libgd.so.2.0.0 ${XLIB_DIR}/libgd.so
}

xclean()
{
   make clean
   make distclean
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
