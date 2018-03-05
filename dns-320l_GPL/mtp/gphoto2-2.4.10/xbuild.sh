#!/bin/sh

unset CFLAGS
unset LDFLAGS
unset LIBS

source ../../xcp.sh

#echo make  libexif, libpopt, libgphoto2  first!
#
#"LIBGPHOTO2_LIBS=-L$MODULES_PATH/mtp/libgphoto2-2.4.10.1/libgphoto2/.libs -lgphoto2" 
#"LIBEXIF_LIBS=-L$MODULES_PATH/mtp/libexif-0.6.20/libexif/.libs" 
#"POPT_LIBS=-L$MODULES_PATH/popt-1.16/.libs -lpopt"

xbuild()
{
	if [ ! -e ../libgphoto2-2.4.10.1/libgphoto2/.libs/libgphoto2.so.2 ]; then
		echo make libgphoto2 first!!!
		return
	fi
	
	if [ ! -e ../../libexif-0.6.20/libexif/.libs/libexif.so.12 ]; then
		echo make libexif first!!!
		return
	fi

	if [ ! -e ../../popt-1.16/.libs/libpopt.so.0 ]; then
		echo make popt-1.16 first!!!
		return
	fi

	make clean ; make distclean
	MODULES_PATH=`pwd`/../..
	./configure --with-libgphoto2=auto --with-libexif=auto --host=${TARGET_HOST} LDFLAGS=-s "LIBGPHOTO2_CFLAGS=-I$MODULES_PATH/mtp/libgphoto2-2.4.10.1 -I$MODULES_PATH/mtp/libgphoto2-2.4.10.1/libgphoto2_port" "LIBGPHOTO2_LIBS=-L$MODULES_PATH/mtp/libgphoto2-2.4.10.1/libgphoto2/.libs -lgphoto2" "LIBEXIF_CFLAGS=-I$MODULES_PATH/mtp/libexif-0.6.20/libexif" "LIBEXIF_LIBS=-L$MODULES_PATH/mtp/libexif-0.6.20/libexif/.libs" "POPT_CFLAGS=-I$MODULES_PATH/popt-1.16" "POPT_LIBS=-L$MODULES_PATH/popt-1.16/.libs -lpopt"
	make
}

xinstall()
{
	${CROSS_COMPILE}strip -s gphoto2/.libs/gphoto2
	xcp gphoto2/.libs/gphoto2 ${ROOT_FS}/sbin
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
	echo "Usage : xbuild.sh {build | install | clean}"
fi
