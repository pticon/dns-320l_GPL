#!/bin/sh

unset CFLAGS
unset LDFLAGS
unset LIBS

source ../xcp.sh

xbuild()
{
	XINSTDIR=$(readlink -f ../_xinstall/${PROJECT_NAME})
   
	if [ ! -e ${XINSTDIR}/usr/lib/libssl.so ]; then
		echo "ERROR: You should build openssl first"
		echo ""
		echo "\$ cd ../zlib-1.2.3"
		echo "\$ ./xbuild.sh build"
		echo ""
		echo "\$ cd ../openssl-1.0.1c"
		echo "\$ ./xbuild.sh build"
		exit 1
	fi
   
	export CPPFLAGS="-I$XINC_DIR" LDFLAGS="-L$XLIB_DIR -liconv -lz"
	./configure --host=${CC%-*} --disable-debug --with-ssl=openssl --with-libssl-prefix="${XINSTDIR}/usr"
	make clean
	make
}

xinstall()
{
	${CROSS_COMPILE}strip -s src/wget
	xcp src/wget ${ROOT_FS}/bin
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
