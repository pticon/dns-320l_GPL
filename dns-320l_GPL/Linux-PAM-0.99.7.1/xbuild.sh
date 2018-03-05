#!/bin/sh

unset CFLAGS
unset LDFLAGS
unset LIBS

source ../xcp.sh
MY_PREFIX=$PWD/../_xinstall/${PROJECT_NAME}

xbuild()
{
	export CFLAGS="${CFLAGS} -I${MY_PREFIX}/include"
	export CPPFLAGS="${CFLAGS} -I${MY_PREFIX}/include"
	export LDFLAGS="${LDFLAGS} -L${MY_PREFIX}/lib"
	export LIBS="-lfl"
	#find . -name Makefile | xargs rm -vf
	./configure  --host=${TARGET_HOST} --prefix=${MY_PREFIX} --with-gnu-ld --disable-nls --includedir=${MY_PREFIX}/include/security

	make 
	make install
}

xinstall()
{
	echo ${MY_PREFIX}
    ${CROSS_COMPILE}strip -v ${MY_PREFIX}/lib/libpam.so.0.81.6 -o ${ROOT_FS}/usrlib/libpam.so.0.81.6
    xcp ${MY_PREFIX}/lib/libpam.so ${ROOT_FS}/usrlib/libpam.so.0.81.6      
    xcp ${MY_PREFIX}/lib/libpam.so.0 ${ROOT_FS}/usrlib/libpam.so.0.81.6
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
	echo "Usage : xbuild.sh {build | install | clean}"
fi
