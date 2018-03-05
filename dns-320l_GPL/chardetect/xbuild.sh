#!/bin/bash

unset CFLAGS
unset LDFLAGS
unset LIBS

source ../xcp.sh

xbuild()
{
	make clean
	make
}

xinstall()
{
	${CROSS_COMPILE}strip -s detect_char_encode
	xcp detect_char_encode ${ROOT_FS}/bin
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
	echo "Usage : xbuild.sh {build | install | clean}"
fi
