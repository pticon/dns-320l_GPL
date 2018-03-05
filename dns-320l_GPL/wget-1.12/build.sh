#!/bin/sh
echo $PWD

PREFIX=$PWD/../

MODULE=$1

#	Andrew_modify_20120807
if [ "$1" == "" ]; then
	echo "Usage: sh build.sh module_name(DNS-345|DNS-320B|DNS-320L|DNS-327L)"
	exit 1
fi

if [ "$MODULE" == "DNS-345" ]; then
	HOST="arm-mv5sft-linux-gnueabi"
	echo "$HOST"

	./configure --host=$HOST --prefix=$PREFIX/xinst --disable-debug host_alias=arm-mv5sft-linux-gnueabi CC=arm-mv5sft-linux-gnueabi-gcc LDFLAGS="-I$PREFIX/xinst -liconv"
fi

#	Andrew_add_20120807
if [ "$MODULE" == "DNS-327L" ]; then
	HOST="arm-marvell-linux-gnueabi"
	echo "$HOST"
	
	./configure --host=$HOST --disable-debug host_alias=arm-marvell-linux-gnueabi CC=arm-marvell-linux-gnueabi-gcc 
fi

make clean;make

