#!/bin/sh

unicenv gpl
#unset CFLAGS
#unset LDFLAGS
#unset LIBS

source ../xcp.sh

xbuild()
{
   make clean
   make clobber
   make
   ${CC%-*}-strip syslogd
}

xinstall()
{
   ${CROSS_COMPILE}strip -s syslogd

   xcp syslogd ${ROOT_FS}/sbin

}

xclean()
{
	make clean
	make clobber

	git checkout klogd syslogd

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

