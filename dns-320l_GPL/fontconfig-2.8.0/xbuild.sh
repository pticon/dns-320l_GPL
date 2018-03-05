#!/bin/bash


export PATH=${PWD}/../gd_cluster/xinst/bin:$PATH
export ICONV_LIBS=${PWD}/../gd_cluster/xinst/lib

source ../xcp.sh

TOP_DIR=$(cd "$(dirname "$0")/.."  ; pwd )

xbuild()
{
	make clean;make distclean

	CFLAGS="${CFLAGS} -I${TOP_DIR}/_xinstall/${PROJECT_NAME}/include -I${TOP_DIR}/_xinstall/${PROJECT_NAME}/include/freetype2" \
	LDFLAGS="${LDFLAGS} -L${TOP_DIR}/_xinstall/${PROJECT_NAME}/lib -s" \
	./configure \
	--build=i386-linux \
	--host=${TARGET_HOST} \
	--with-arch=ARM \
	--enable-libxml2 \
	--prefix=${TOP_DIR}/_xinstall/${PROJECT_NAME} \
	--enable-libxml2 \
	--with-freetype-config=${TOP_DIR}/_xinstall/${PROJECT_NAME}/bin/freetype-config \
	--with-expat=${TOP_DIR}/_xinstall/${PROJECT_NAME} \
	LIBXML2_CFLAGS="-I${TOP_DIR}/_xinstall/${PROJECT_NAME}/include/libxml2" \
	LIBXML2_LIBS="-L${TOP_DIR}/_xinstall/${PROJECT_NAME}/lib -lxml2" 

	make
	make install
}

xinstall()
{
   ${CROSS_COMPILE}strip -s ./src/.libs/libfontconfig.so.1.4.4
   
   xcp ./src/.libs/libfontconfig.so.1.4.4 ${ROOT_FS}/lib/libfontconfig.so.1
   xcp ./src/.libs/libfontconfig.so.1.4.4 ${XLIB_DIR}/libfontconfig.so
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
