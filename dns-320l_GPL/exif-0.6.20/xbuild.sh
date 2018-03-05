#!/bin/sh

if [ ! -e $PWD/../_xinstall/${PROJECT_NAME}/lib/libpopt.so ]; then
	echo ""
	echo -e "\033[31m *ERROR*: You need to build popt-1.16 first. \033[0m"
	echo ""
	echo "\$ cd ../popt-1.16/"
	echo "\$ sh xbuild.sh"
	echo ""
	exit 1
fi

if [ ! -e $PWD/../libexif-0.6.20/_install/lib/libexif.so ]; then
	echo ""
	echo -e "\033[31m *ERROR*: You need to build libexif-0.6.20 first. \033[0m"
	echo ""
	echo "\$ cd ../libexif-0.6.20/"
	echo "\$ sh xbuild.sh"
	echo ""
	exit 1
fi

./configure --host=arm-linux --prefix=$PWD/_install CPPFLAGS=-I$PWD/../_xinstall/${PROJECT_NAME}/include POPT_LIBS=$PWD/../_xinstall/${PROJECT_NAME}/lib/libpopt.so POPT_CFLAGS=$PWD/../_xinstall/${PROJECT_NAME}/lib PKG_CONFIG_PATH=$PWD/../libexif-0.6.20/_install/lib/pkgconfig

make
make install
