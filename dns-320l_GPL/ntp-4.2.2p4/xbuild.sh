#!/bin/sh

unset CFLAGS
unset LDFLAGS
unset LIBS

source ../xcp.sh

xbuild()
{
	#export CPPFLAGS="-I$XINC_DIR/openssl-1.0.1c -I$XINC_DIR" LDFLAGS="-L$XLIB_DIR -lxmldbc"
	#./configure --host=${CC%-*} --prefix=$(readlink -f $PWD/../_xinstall/${PROJECT_NAME}) --with-sntp --with-openssl-libdir=$XLIB_DIR --with-openssl-incdir=$XINC_DIR
	./configure --host=${CC%-*} --prefix=$(readlink -f $PWD/../_xinstall/${PROJECT_NAME}) --with-sntp
	cd sntp
	make clean;make
	cd ..
}

xinstall()
{
	${CROSS_COMPILE}strip -s sntp/sntp
	xcp sntp/sntp ${ROOT_FS}/sbin
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
   echo "Usage : xbuild.sh build or xbuild.sh install or xbuild.sh clean"
fi