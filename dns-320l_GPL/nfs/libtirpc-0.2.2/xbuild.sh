#!/bin/sh

unset CFLAGS
unset LDFLAGS
unset CPPFLAGS
unset LIBS

source ../../xcp.sh

xbuild()
{
	if [ ! -e ../libgssglue-0.4/xinst/lib/libgssglue.a ]; then
		cat <<-EOF
	
		ERROR: "../libgssglue-0.4/xinst/lib/libgssglue.a" does not exist!
		Please build it first.
	
		$ cd ../libgssglue-0.4
		$ ./xbuild build
		$ ./xbuild install
	
		EOF
	
		exit 1
	fi

	make clean
	make distclean
    ./configure --host=arm-linux GSSGLUE_CFLAGS=${PWD}/../libgssglue-0.4/xinst/include GSSGLUE_LIBS=${PWD}/../libgssglue-0.4/xinst/lib
	make
}

xinstall()
{
	make DESTDIR=${PWD}/xinst install

    ${CROSS_COMPILE}strip -s ${PWD}/xinst/usr/lib/libtirpc.so.1.0.10
	
	xcp ${PWD}/xinst/usr/lib/libtirpc.so.1.0.10 ${ROOT_FS}/lib
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

