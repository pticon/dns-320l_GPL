#!/bin/sh

PREFIX=`basename $CC -gcc`
CXX=${PREFIX}-g++
STRIP=${PREFIX}-strip
LD=${PREFIX}-ld
RANLIB=${PREFIX}-ranlib
AR=${PREFIX}-ar

echo ""
echo "try to make distclean"
echo ""

make distclean > /dev/null 2>&1

./configure --host=arm-linux-gnu --prefix=${PWD}/.install

echo ""
echo "Now, we are making"
echo ""

make

if [ $? = 0 ]; then
	$STRIP ./.libs/libusb-0.1.so.4.4.4
fi
