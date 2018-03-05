#!/bin/sh
echo $PWD

PREFIX=$PWD/../

MODULE=$1

if [ "$1" == "" ]; then
	echo "Usage: sh build.sh module_name(DNS-345|DNS-320)"
	exit 1
fi

if [ "$MODULE" == "DNS-345" ]; then
	HOST="arm-mv5sft-linux-gnueabi"
	echo "$HOST"
fi

./configure --host=$HOST --prefix=$PREFIX/xinst --disable-debug host_alias=arm-mv5sft-linux-gnueabi CC=arm-mv5sft-linux-gnueabi-gcc LDFLAGS="-I$PREFIX/xinst -liconv"

make clean;make

