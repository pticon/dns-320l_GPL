#!/bin/sh

unset CFLAGS
unset LDFLAGS
unset LIBS

source ../xcp.sh

xbuild()
{
	rm -rf build
	mkdir build
	cd build
	cmake ..
	make
	cp -avf yajl-2.0.1/* $PWD/../../_xinstall/${PROJECT_NAME}
	cd ..
}

xinstall()
{
   ${CROSS_COMPILE}strip -s build/yajl-2.0.1/lib/libyajl.so.2.0.1
  
   xcp build/yajl-2.0.1/lib/libyajl.so.2.0.1  ${ROOT_FS}/lib
   xcp build/yajl-2.0.1/lib/libyajl.so  ${ROOT_FS}/lib
   xcp build/yajl-2.0.1/lib/libyajl.so.2  ${ROOT_FS}/lib
}

xclean()
{
   rm -rf build
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
