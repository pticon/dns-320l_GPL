#!/bin/sh

unset CFLAGS
unset LDFLAGS
unset CPPFLAGS
unset LIBS


xbuild()
{
	make clean
	make distclean
	./configure --host=arm-linux --prefix=${PWD}/xinst
	make
}

xinstall()
{
	make install
}

xclean()
{
	make clean
	make distclean
	rm -rf xinst
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

