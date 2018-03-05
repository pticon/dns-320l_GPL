#!/bin/sh

INST_TMP=$(readlink -f $PWD/../_xinstall/${PROJECT_NAME})

xbuild()
{
	make clean
	make distclean

	./configure
	
	make
}

xinstall()
{
	make prefix=${INST_TMP} install
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
