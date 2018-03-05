#!/bin/bash


source ../xcp.sh

xbuild()
{
	./configure \
	--build=i386-linux \
	--host=${TARGET_HOST} \
	--prefix=${PWD}/../_xinstall/${PROJECT_NAME} \
	--oldincludedir=${PWD}/../_xinstall/${PROJECT_NAME}

	make
}

xinstall()
{
	make install

	${CROSS_COMPILE}strip -s ../_xinstall/${PROJECT_NAME}/lib/libfreetype.so.6.3.20

	xcp ../_xinstall/${PROJECT_NAME}/lib/libfreetype.so.6.3.20 ${ROOT_FS}/lib/libfreetype.so.6
	xcp ../_xinstall/${PROJECT_NAME}/lib/libfreetype.so.6.3.20 ${XLIB_DIR}/libfreetype.so
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
