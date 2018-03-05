#!/bin/bash

INST_TMP=$(readlink -f $PWD/../_xinstall/${PROJECT_NAME})

xbuild()
{
./configure --host=arm-linux \
ac_cv_file___dev_urandom_=yes \
LDFLAGS="-L${INST_TMP}/lib" \
CPPFLAGS="-I${INST_TMP}/include" \
--with-libmcrypt-prefix="${INST_TMP}" \
--prefix=${INST_TMP}


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

	xcp ./lib/.libs/libmcrypt.so.4.4.8 ${ROOT_FS}/lib/libmhash.so.2.0.1
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
