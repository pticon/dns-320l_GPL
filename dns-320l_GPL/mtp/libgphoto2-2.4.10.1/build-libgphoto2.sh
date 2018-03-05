#!/bin/sh
echo make  libltdl, libusb, libexif  first!

make clean ; make distclean

MODULES_PATH=`pwd`/../..
./configure --host=arm-gnu-linux --with-libexif="auto" CPPFLAGS= LDFLAGS=-s "LTDLINCL=-I$MODULES_PATH/libtool-1.5.6/libltdl" "LIBLTDL=-L$MODULES_PATH/libtool-1.5.6/libltdl/.libs -lltdl" "LIBUSB_CFLAGS=-I$MODULES_PATH/libusb-0.1.12" "LIBUSB_LIBS=-L$MODULES_PATH/libusb-0.1.12/.libs -lusb" "LIBEXIF_CFLAGS=-I$MODULES_PATH/mtp/libexif-0.6.20/libexif" "LIBEXIF_LIBS=-L$MODULES_PATH/mtp/libexif-0.6.20/libexif/.libs" "CFLAGS=-I$MODULES_PATH/jpeg-7"

make
