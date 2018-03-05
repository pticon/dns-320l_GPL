#!/bin/sh

unset CFLAGS
unset LDFLAGS
unset LIBS

source ../xcp.sh

xbuild()
{
	make clean
	make LDFLAGS="${LDFLAGS} -L${XLIB_DIR} -L${XLIB_DIR}/mysql"
}
xinstall()
{
	${CROSS_COMPILE}strip -s ./logrotate
	xcp ./logrotate ${ROOT_FS}/sbin
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
