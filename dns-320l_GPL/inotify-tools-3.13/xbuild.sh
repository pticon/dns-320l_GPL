#!/bin/sh

unset CFLAGS
unset LDFLAGS
unset LIBS

source ../xcp.sh

xbuild()
{

	find ./ * | xargs touch -d `date -d 'today' +%y%m%d`
	[ -e Makefile ] || ./configure --build=i386-linux --host=${TARGET_HOST} --prefix=$(readlink -f $PWD/../_xinstall/${PROJECT_NAME})
	make clean
	make
	make install
}

xinstall()
{
	${CROSS_COMPILE}strip -s $(readlink -f $PWD/../_xinstall/${PROJECT_NAME})/lib/libinotifytools.so.0.4.1
	cp $(readlink -f $PWD/../_xinstall/${PROJECT_NAME})/lib/libinotifytools.so ${XLIB_DIR}
	cp $(readlink -f $PWD/../_xinstall/${PROJECT_NAME})/lib/libinotifytools.so.0 ${XLIB_DIR}

	cp $(readlink -f $PWD/../_xinstall/${PROJECT_NAME})/lib/libinotifytools.so ${ROOT_FS}/lib
	cp $(readlink -f $PWD/../_xinstall/${PROJECT_NAME})/lib/libinotifytools.so.0 ${ROOT_FS}/lib
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
   echo "Usage : [xbuild.sh build] or [xbuild.sh install] or [xbuild.sh clean]"
fi

