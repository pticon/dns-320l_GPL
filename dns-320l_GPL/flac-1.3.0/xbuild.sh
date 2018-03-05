#!/bin/sh



case $PROJECT_NAME in

	DNS-327L)
	;;
	LIGHTNING-4A)
	;;
	KingsCanyon)
	;;

    *)
	;;

esac

unset CFLAGS
unset LDFLAGS
unset LIBS

[ -d "${0%/*}" ] && cd ${0%/*}
source ../xcp.sh
MY_PREFIX=$PWD/../_xinstall/${PROJECT_NAME}
xbuild()
{
	[ -e Makefile ] || ./configure --host=${CC%-*} --prefix=${MY_PREFIX} --without-ogg
	make
	make install
	${CC%-*}-strip ${MY_PREFIX}/lib/libFLAC.so.8
	${CC%-*}-strip ${MY_PREFIX}/bin/flac
}

xinstall()
{

	xcp ${MY_PREFIX}/lib/libFLAC.so.8 ${ROOT_FS}/lib
	xcp ${MY_PREFIX}/lib/libFLAC.so ${XLIB_DIR}
	xcp ${MY_PREFIX}/bin/flac ${ROOT_FS}/bin
}

xclean()
{
	make distclean
	rm -r `pwd`/autom4te.cache
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

