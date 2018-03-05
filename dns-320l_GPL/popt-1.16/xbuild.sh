#!/bin/sh

unset CFLAGS
unset LDFLAGS
unset LIBS

source ../xcp.sh

xbuild()
{
	#./configure --host=${TARGET_HOST}
	#make
	mkdir _install
	./configure --build=i386-linux --host=arm-marvell-linux-gnueabi --prefix=$PWD/_install
	make
	make install
}

xinstall()
{
	cp -avf ./_install/lib/libpopt.so ${ROOT_FS}/lib/libpopt.so
	cp -avf ./_install/lib/libpopt.so.0 ${ROOT_FS}/lib/libpopt.so.0
	cp -avf ./_install/lib/libpopt.so.0.0.0 ${ROOT_FS}/lib/libpopt.so.0.0.0

    cp -avf ./_install/lib/libpopt.so ${XLIB_DIR}/libpopt.so
    cp -avf ./_install/lib/libpopt.so.0 ${XLIB_DIR}/libpopt.so.0
    cp -avf ./_install/lib/libpopt.so.0.0.0 ${XLIB_DIR}/libpopt.so.0.0.0

	cp -avf ./_install/include/*	${XINC_DIR}
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
