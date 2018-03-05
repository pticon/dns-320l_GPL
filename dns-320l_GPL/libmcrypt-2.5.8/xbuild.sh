#!/bin/bash

source ../xcp.sh

xbuild()
{
./configure \
--host=arm-linux \
--prefix="`pwd`/../_xinstall/${PROJECT_NAME}" \
ac_cv_func_malloc_0_nonnull=yes \
ac_cv_func_realloc_0_nonnull=yes

if [ $? != 0 ] ; 
then echo "configure failed!!!!"
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
	${CROSS_COMPILE}strip -s ./lib/.libs/libmcrypt.so.4.4.8
	xcp ./lib/.libs/libmcrypt.so.4.4.8 ${ROOT_FS}/lib/libmcrypt.so.4.4.8
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

