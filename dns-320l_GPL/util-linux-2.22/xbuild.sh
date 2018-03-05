#!/bin/sh

unset CFLAGS
unset LDFLAGS
unset LIBS

source ../xcp.sh

xbuild()
{
   export CPPFLAGS=-I$XINC_DIR LDFLAGS="-L$XLIB_DIR"
   ./configure --host=${TARGET_HOST} --without-ncurses --disable-login --disable-su --prefix=$(readlink -f $PWD/../_xinstall/${PROJECT_NAME})
   make clean
   make
}

xinstall()
{
   ${CROSS_COMPILE}strip -s disk-utils/blockdev
   ${CROSS_COMPILE}strip -s mount/mount
   ${CROSS_COMPILE}strip -s mount/umount
   
   xcp mount/mount ${ROOTDIR}/ramdisk/${PROJECT_NAME}/bin
   xcp mount/umount ${ROOTDIR}/ramdisk/${PROJECT_NAME}/bin
   xcp disk-utils/blockdev ${ROOT_FS}/bin
   
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
