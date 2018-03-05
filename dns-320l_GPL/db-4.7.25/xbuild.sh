#!/bin/sh

unset CFLAGS
unset LDFLAGS
unset LIBS

source ../xcp.sh

xbuild()
{
   cd build_unix
   ../dist/configure --host=${TARGET_HOST} --prefix=$(readlink -f $PWD/../../_xinstall/${PROJECT_NAME}) --enable-pthread_api --enable-rpc
   make clean
   make
   make install
}

xinstall()
{
   ${CROSS_COMPILE}strip -s build_unix/.libs/libdb-4.7.so
   
   xcp build_unix/.libs/libdb-4.7.so ${ROOT_FS}/lib
}

xclean()
{
	cd build_unix
 	make clean
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

