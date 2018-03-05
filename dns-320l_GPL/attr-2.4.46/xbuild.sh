#!/bin/sh

unset CFLAGS
unset LDFLAGS
unset LIBS

source ../xcp.sh
MY_PREFIX=$PWD/../_xinstall/${PROJECT_NAME}

xbuild()
{
   ./configure --host=${TARGET_HOST} --prefix=${MY_PREFIX} --enable-gettext=no --enable-shared=yes
   make
   make install install-lib install-dev 
}

xinstall()
{
   ${CROSS_COMPILE}strip -s libattr/.libs/libattr.so.1
   
   xcp libattr/.libs/libattr.so.1 ${ROOT_FS}/lib
   xcp libattr/.libs/libattr.so.1 ${XLIB_DIR}/lib
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




if [ "$1" = "build" ]; then
   xbuild
elif [ "$1" = "install" ]; then
   xinstall
elif [ "$1" = "clean" ]; then
   xclean
else
   echo "Usage : [xbuild.sh build] or [xbuild.sh install] or [xbuild.sh clean]"
fi
