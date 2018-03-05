#!/bin/sh

unset CFLAGS
unset LDFLAGS
unset LIBS

source ../../xcp.sh

xbuild()
{
	make clean

	rm -rf autom4te.cache/
	make clean 2>/dev/null

	autoreconf -v -f -i
	./configure --host=${TARGET_HOST} CFLAGS="-O2 -I${XINC_DIR}" LDFLAGS="-L${XLIB_DIR} -lxmldbc"

	make
}

xinstall()
{
	${CROSS_COMPILE}strip -s ez-ipupdate
	xcp ez-ipupdate ${ROOT_FS}/bin
}

xclean()
{
	make clean
#	./clean.sh
}

if [ "$1" = "build" ]; then
	xbuild
elif [ "$1" = "install" ]; then
	xinstall
elif [ "$1" = "clean" ]; then
	xclean
else
	echo "Usage : xbuild.sh {build | install | clean}"
fi
