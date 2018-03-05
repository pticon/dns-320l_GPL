#!/bin/sh

unset CFLAGS
unset LDFLAGS
unset CPPFLAGS
unset LIBS

source ../../xcp.sh

xbuild()
{
	if [ ! -e ../libtirpc-0.2.2/xinst/usr/lib/libtirpc.so.1 ]; then
		cat <<-EOF
	
		ERROR: "../libtirpc-0.2.2/xinst/usr/lib/libtirpc.so.1" does not exist!
		Please build it first.
	
		$ cd ../libtirpc-0.2.2
		$ ./xbuild build
		$ ./xbuild install
	
		EOF
	
		exit 1
	fi

	make clean
	make distclean
	./configure --host=arm-linux --prefix=${PWD}/xinst CFLAGS=-I${PWD}/../libtirpc-0.2.2/xinst/usr/include LDFLAGS=-L${PWD}/../libtirpc-0.2.2/xinst/usr/lib
	make
}

xinstall()
{
	make install
	${CROSS_COMPILE}strip -s ${PWD}/xinst/bin/*
	xcp ${PWD}/xinst/bin/* ${ROOT_FS}/bin
}

xclean()
{
	make clean
	make distclean
	rm -rf xinst
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

