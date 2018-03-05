#!/bin/sh

unset CFLAGS
unset LDFLAGS
unset LIBS

source ../xcp.sh

xbuild()
{

	if [ ! -e $(readlink -f $PWD/../_xinstall/${PROJECT_NAME})/lib/libncurses.so.5.5 ]; then
		echo "We need ncurses to compile"
		exit 1
	fi
    
	make clean
    make distclean

	CFLAGS="-I$(readlink -f $PWD/../_xinstall/${PROJECT_NAME})/include/ncurses" \
		LDFLAGS="-L$(readlink -f $PWD/../_xinstall/${PROJECT_NAME})/lib -lncurses" \
	./configure --host=arm-linux --with-curses --prefix=$(readlink -f $PWD/../_xinstall/${PROJECT_NAME})

   make
   make install
}

xinstall()
{
   ${CROSS_COMPILE}strip -s shlib/libreadline.so.6.2 
   
   cp shlib/libreadline.so.6.2 ${ROOT_FS}/lib/libreadline.so.6
}

xclean()
{
   make clean ; make distclean
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
