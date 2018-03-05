#!/bin/sh

unset CFLAGS
unset LDFLAGS
unset LIBS

source ../xcp.sh

xbuild()
{
   make clean
   ./configure --prefix=$(readlink -f $PWD/../_xinstall/${PROJECT_NAME}) --host=${TARGET_HOST} --disable-nls --enable-elf-shlibs --disable-testio-debug
   make
   make install
}

xinstall()
{
   ${CROSS_COMPILE}strip -s lib/libblkid.so.1.0
   ${CROSS_COMPILE}strip -s lib/libcom_err.so.2.1
   ${CROSS_COMPILE}strip -s lib/libss.so.2.0
   ${CROSS_COMPILE}strip -s lib/libuuid.so.1.2
   
   xcp lib/libblkid.so.1.0 ${ROOT_FS}/lib
   xcp lib/libcom_err.so.2.1 ${ROOT_FS}/lib
   xcp lib/libss.so.2.0 ${ROOT_FS}/lib
   xcp lib/libuuid.so.1.2 ${ROOT_FS}/lib
}

xclean()
{
   make clean
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
