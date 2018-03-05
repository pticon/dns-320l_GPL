#!/bin/sh

if [ ! -e $PWD/../LVM2.2.02.85/xinst/lib/libdevmapper.so ]; then
	echo ""
	echo "*ERROR*: You need to build LVM first."
	echo ""
	echo "\$ cd ../LVM2.2.02.85/"
	echo "\$ sh xbuild.sh"
	echo ""
	exit 1
fi

if [ ! -e $PWD/../libgpg-error-1.10/xinst/lib/libgpg-error.so ]; then
	echo ""
	echo "*ERROR*: You need to build libgpg-error first."
	echo ""
	echo "\$ cd ../libgpg-error-1.10/"
	echo "\$ sh xbuild.sh"
	echo ""
	exit 1
fi

if [ ! -e $PWD/../e2fsprogs-libs-1.41.14/xinst/lib/libuuid.so ]; then
	echo ""
	echo "*ERROR*: You need to build e2fsprogs-libs (libuuid) first."
	echo ""
	echo "\$ cd ../e2fsprogs-libs-1.41.14/"
	echo "\$ sh xbuild.sh"
	echo ""
	exit 1
fi

if [ ! -e $PWD/../popt-1.16/xinst/lib/libpopt.so ]; then
	echo ""
	echo "*ERROR*: You need to build popt first."
	echo ""
	echo "\$ cd ../popt-1.16/"
	echo "\$ sh xbuild.sh"
	echo ""
	exit 1
fi


./configure --prefix=$PWD/xinst --host=arm-linux --disable-nls --disable-selinux --disable-udev CFLAGS="-I$PWD/../LVM2.2.02.85/xinst/include -I$PWD/../libgpg-error-1.10/xinst/include -I$PWD/../libgcrypt-1.5.0-beta1/xinst/include -I$PWD/../e2fsprogs-libs-1.41.14/xinst/include -I$PWD/../popt-1.16/xinst/include" LDFLAGS="-L$PWD/../LVM2.2.02.85/xinst/lib -L$PWD/../libgpg-error-1.10/xinst/lib -L$PWD/../libgcrypt-1.5.0-beta1/xinst/lib -L$PWD/../e2fsprogs-libs-1.41.14/xinst/lib -L$PWD/../popt-1.16/xinst/lib"

make
make install
