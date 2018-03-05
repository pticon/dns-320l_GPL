#!/bin/sh

xbuild()
{
	make clean
	make distclean
	./configure --host=arm-none-linux --prefix=$(readlink -f $PWD/../_xinstall/${PROJECT_NAME}) --with-shared
	make
	make install
}

xinstall()
{
	${CROSS_COMPILE}strip -s  $(readlink -f $PWD/../_xinstall/${PROJECT_NAME})/lib/libncurses.so.5.5
	
	cp $(readlink -f $PWD/../_xinstall/${PROJECT_NAME})/lib/libncurses.so.5.5 ${ROOT_FS}/lib/
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
	echo "Usage: xbuild.sh {build|install|clean}"
fi
