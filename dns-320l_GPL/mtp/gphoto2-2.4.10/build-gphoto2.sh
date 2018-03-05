#!/bin/sh
echo make  libexif, libpopt, libgphoto2  first!

make clean ; make distclean

MODULES_PATH=`pwd`/../..
./configure --with-libgphoto2=auto --with-libexif=auto --host=arm-linux-gnu LDFLAGS=-s "LIBGPHOTO2_CFLAGS=-I$MODULES_PATH/mtp/libgphoto2-2.4.10.1 -I$MODULES_PATH/mtp/libgphoto2-2.4.10.1/libgphoto2_port" "LIBGPHOTO2_LIBS=-L$MODULES_PATH/mtp/libgphoto2-2.4.10.1/libgphoto2/.libs -lgphoto2" "LIBEXIF_CFLAGS=-I$MODULES_PATH/mtp/libexif-0.6.20/libexif" "LIBEXIF_LIBS=-L$MODULES_PATH/mtp/libexif-0.6.20/libexif/.libs" "POPT_CFLAGS=-I$MODULES_PATH/mtp/popt-1.16" "POPT_LIBS=-L$MODULES_PATH/mtp/popt-1.16/.libs -lpopt"

make
