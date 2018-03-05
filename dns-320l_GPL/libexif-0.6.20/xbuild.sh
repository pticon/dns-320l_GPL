#!/bin/sh
unset CFLAGS
unset LDFLAGS
unset LIBS

source ../xcp.sh

build()
{
	./configure --host=${TARGET_HOST} --prefix=$PWD/_install

	make
	make install
}

install()
{
	${CROSS_COMPILE}strip $PWD/_install/lib/libexif.so.12
	
	xcp $PWD/_install/lib/libexif.so.12 ${ROOT_FS}/lib
}

clean()
{
	make clean
}

if [ "$1" = "build" ]; then
	build
elif [ "$1" = "install" ]; then
	install
elif [ "$1" = "clean" ]; then
	clean
else
	echo "Usage : $0 build or $0 install or $0 clean"
fi
