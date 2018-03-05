#!/bin/sh

source ../xcp.sh

xbuild(){
	make clean; make
}

xinstall()
{	
	echo "install"
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