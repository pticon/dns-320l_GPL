#!/bin/sh

INST_TMP=$(readlink -f $PWD/../_xinstall/${PROJECT_NAME})

xbuild()
{
	make clean

	make REAL_DAEMON_DIR=/usr/lib STYLE=-DSYS_ERRLIST_DEFINED linux
}

xinstall()
{
	echo ""
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
	echo "Usage: xbuild.sh {build|install|clean}"
fi
