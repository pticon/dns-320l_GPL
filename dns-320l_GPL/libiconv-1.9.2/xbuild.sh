#!/bin/bash

unset CFLAGS
unset LDFLAGS
unset LIBS

source ../xcp.sh
MY_PREFIX=$PWD/../_xinstall/${PROJECT_NAME}

xbuild()
{
	#if [ $PROJECT_NAME == "DNS-320L" -o $PROJECT_NAME == "DNS-320B" -o $PROJECT_NAME == "DNS-345" ]; then
	
	#if [ ! -e $MY_PREFIX/lib/libssl.so ]; then
	#cat <<-EOF

	#ERROR: "$MY_PREFIX/lib/libssl.so" does not exist!
	#Please build it first.
	#EOF

	#exit 1
	#fi
	
	#else
	
	##build openssl-1.0.1c first
	#if [ ! -e $MY_PREFIX/usr/lib/libssl.so.1.0.0 ]; then
	#cat <<-EOF

	#ERROR: "$MY_PREFIX/usr/lib/libssl.so.1.0.0" does not exist!
	#Please build it first.

	#EOF

	#exit 1
	#fi
	
	#fi
	
   ./configure --host=${TARGET_HOST} --prefix=${MY_PREFIX} --enable-extra-encodings
   make
   make install
}

xinstall()
{
   ${CROSS_COMPILE}strip -s lib/.libs/libiconv.so.2
   
   xcp lib/.libs/libiconv.so.2 ${ROOT_FS}/lib

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
   echo "Usage : [xbuild.sh build] or [xbuild.sh install] or [xbuild.sh clean]"
fi

