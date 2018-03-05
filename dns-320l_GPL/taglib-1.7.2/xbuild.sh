#!/bin/sh

unset CFLAGS
unset LDFLAGS
unset LIBS

source ../xcp.sh

PREFIX=$(readlink -f $PWD/../_xinstall/${PROJECT_NAME})

xbuild()
{
	mkdir -p xbuild
	cd xbuild/
	cmake -DCMAKE_INSTALL_PREFIX=$PREFIX -DZLIB_INCLUDE_DIR=$PREFIX/include/ -DZLIB_LIBRARY=$PREFIX/lib/libz.so -DCMAKE_RELEASE_TYPE=Release -DCMAKE_BUILD_TYPE=Release ..
	make
	make install
}

xinstall()
{
	${CROSS_COMPILE}strip ./xbuild/taglib/libtag.so.1.7.2
	
	xcp ./xbuild/taglib/libtag.so.1.7.2 ${ROOT_FS}/lib/libtag.so.1.7.2
	xcp ./xbuild/taglib/libtag.so ${XLIB_DIR}/libtag.so
	xcp ./xbuild/taglib/libtag.so.1 ${XLIB_DIR}/libtag.so.1
	xcp ./xbuild/bindings/c/libtag_c.so.0.0.0 ${ROOT_FS}/lib/libtag_c.so.0.0.0
	xcp ./xbuild/bindings/c/libtag_c.so.0.0.0 ${XLIB_DIR}/libtag_c.so.0.0.0
	
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
