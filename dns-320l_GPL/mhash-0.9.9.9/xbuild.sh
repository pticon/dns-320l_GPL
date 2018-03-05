#!/bin/bash

source ../xcp.sh

xbuild()
{
./configure \
--host=${TARGET_HOST} \
--prefix="`pwd`/../_xinstall/${PROJECT_NAME}" \
ac_cv_file___dev_urandom_=yes \
ac_cv_func_malloc_0_nonnull=yes \
ac_cv_func_realloc_0_nonnull=yes 

if [ $? != 0 ] ; then
	echo "configure failed!!!!"
	exit 1
fi

make clean
make

if [ $? != 0 ] ; then
	echo "make failed!!!!"
	exit 1
fi

make install

if [ $? != 0 ] ; then
	echo "make install failed!!!!"
	exit 1
fi

}

xinstall()
{
	${CROSS_COMPILE}strip -s ./lib/.libs/libmhash.so.2.0.1

	xcp ./lib/.libs/libmhash.so.2.0.1 ${ROOT_FS}/lib/libmhash.so.2.0.1
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
