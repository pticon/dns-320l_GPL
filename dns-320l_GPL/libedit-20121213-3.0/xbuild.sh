#!/bin/sh


source ../xcp.sh


TMPINST=$(readlink -f $PWD/../_xinstall/${PROJECT_NAME})

xbuild()
{
	make clean
	make distclean

	CFLAGS="-I${TMPINST}/include" \
	LDFLAGS="-L${TMPINST}/lib" \
	./configure --host=arm-linux --prefix=${TMPINST}

	make
	make install
}

xinstall()
{
	${STRIP} -s ${TMPINST}/lib/libedit.so.0.0.42
	xcp ${TMPINST}/lib/libedit.so.0.0.42 ${ROOT_FS}/lib/libedit.so.0
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
	echo "Usage: xbuild.sh {build|install|clean}"
fi
