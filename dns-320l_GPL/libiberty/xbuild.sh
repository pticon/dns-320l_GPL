#!/bin/sh

unset CFLAGS
unset LDFLAGS
unset LIBS

source ../xcp.sh

xbuild()
{
	rm -rf xinst/
	mkdir xinst
	
	export CPPFLAGS="-I../_xinstall/${PROJECT_NAME}/include" LDFLAGS="-L../_xinstall/${PROJECT_NAME}/lib"
	./configure --host=${CC%-*} --prefix=$(readlink -f ../_xinstall/${PROJECT_NAME})
	make clean
	make
}

xclean()
{
	make clean
}

if [ "$1" = "build" ]; then
	xbuild
elif [ "$1" = "clean" ]; then
	xclean
else
	echo "Usage : xbuild.sh build or xbuild.sh install or xbuild.sh clean"
fi
