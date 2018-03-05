#!/bin/sh

unset CFLAGS
unset LDFLAGS
unset LIBS

source ../../xcp.sh

xbuild()
{
	if [ ! -e ../../libtool-1.5.6/libltdl/.libs/libltdl.so.3 ]; then
		echo make libltdl first!!!
		return
	fi
	
	if [ ! -e ../../libusb-0.1.12/.libs/libusb-0.1.so.4 ]; then
		echo make libusb first!!!
		return
	fi

	if [ ! -e ../../libexif-0.6.20/libexif/.libs/libexif.so.12 ]; then
		echo make libexif first!!!
		return
	fi

	if [ ! -e ../../jpeg-7/jconfig.h ]; then
		echo make jpeg-7 first!!!
		return
	fi


	make clean ; make distclean
	XINST_DIR=$(readlink -f $PWD/xinst)
	./configure --prefix=${XINST_DIR} \
		--host=arm-gnu-linux --with-libexif="auto" CPPFLAGS= LDFLAGS=-s \
		LTDLINCL="-I$(readlink -f ../../libtool-1.5.6/libltdl)" \
		LIBLTDL="-L$(readlink -f ../../libtool-1.5.6/libltdl/.libs) -lltdl" \
		LIBUSB_CFLAGS="-I$(readlink -f ../../libusb-0.1.12)" \
		LIBUSB_LIBS="-L$(readlink -f ../../libusb-0.1.12/.libs) -lusb" \
		LIBEXIF_CFLAGS="-I$(readlink -f ../../libexif-0.6.20/libexif)" \
		LIBEXIF_LIBS="-L$(readlink -f ../../libexif-0.6.20/libexif/.libs)" \
		CFLAGS="${CFLAGS} -I$(readlink -f ../../jpeg-7)"
	cat Makefile | sed 's/\t$(LN_S) . $(DESTDIR)$(includedir)/#\t$(LN_S) . $(DESTDIR)$(includedir)/g' > Makefile.new
	mv Makefile.new Makefile 

	make
	make install
}

xinstall()
{
	${CROSS_COMPILE}strip -s libgphoto2/.libs/libgphoto2.so.6
	xcp libgphoto2/.libs/libgphoto2.so.6 ${ROOT_FS}/lib

	${CROSS_COMPILE}strip -s libgphoto2_port/libgphoto2_port/.libs/libgphoto2_port.so.10
	xcp libgphoto2_port/libgphoto2_port/.libs/libgphoto2_port.so.10 ${ROOT_FS}/lib

	${CROSS_COMPILE}strip -s packaging/generic/.libs/print-camera-list
	xcp packaging/generic/.libs/print-camera-list ${ROOT_FS}/sbin

#-	${CROSS_COMPILE}strip -s libgphoto2_port/usb/.libs/check-mtp-device
#-	xcp libgphoto2_port/usb/.libs/check-mtp-device ${ROOT_FS}/sbin

	${CROSS_COMPILE}strip -s libgphoto2_port/.libs/usb.so
	xcp libgphoto2_port/.libs/usb.so ${ROOT_FS}/lib/iolibs

	${CROSS_COMPILE}strip -s libgphoto2_port/.libs/ptpip.so
	xcp libgphoto2_port/.libs/ptpip.so ${ROOT_FS}/lib/iolibs

	${CROSS_COMPILE}strip -s libgphoto2_port/.libs/serial.so
	xcp libgphoto2_port/.libs/serial.so ${ROOT_FS}/lib/iolibs

	${CROSS_COMPILE}strip -s libgphoto2_port/.libs/disk.so
	xcp libgphoto2_port/.libs/disk.so ${ROOT_FS}/lib/iolibs

	${CROSS_COMPILE}strip -s libgphoto2_port/test/.libs/test-gp-port
	xcp libgphoto2_port/test/.libs/test-gp-port ${ROOT_FS}/sbin

	${CROSS_COMPILE}strip -s tests/.libs/test-filesys
	xcp tests/.libs/test-filesys ${ROOT_FS}/sbin

	${CROSS_COMPILE}strip -s tests/.libs/test-gphoto2
	xcp tests/.libs/test-gphoto2 ${ROOT_FS}/sbin

	xcp packaging/linux-hotplug/usbcam.console ${ROOT_FS_COMMON}/files/
	xcp packaging/linux-hotplug/usbcam.group   ${ROOT_FS_COMMON}/files/
	xcp packaging/linux-hotplug/usbcam.user    ${ROOT_FS_COMMON}/files/

	${CROSS_COMPILE}strip -s camlibs/.libs/*.so
#	xcp camlibs/.libs/*.so ${ROOT_FS}/lib/camlibs
	cp camlibs/.libs/*.so ${ROOT_FS}/lib/camlibs
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
