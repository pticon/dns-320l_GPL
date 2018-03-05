#!/bin/bash

unset CFLAGS
unset LDFLAGS
unset LIBS

source ../xcp.sh

CXX=${CROSS_COMPILE}g++
STRIP=${CROSS_COMPILE}strip
LD=${CROSS_COMPILE}ld
RANLIB=${CROSS_COMPILE}ranlib
AR=${CROSS_COMPILE}ar

GPL_PREFIX=${PWD}/../_xinstall/${PROJECT_NAME}
mkdir -p ${GPL_PREFIX}

xbuild()
{
	if [ ! -e ${PWD}/../openssl-1.0.1c/xinst/usr/lib/libssl.so ]; then
		echo ""
		echo -e "\033[31m *ERROR*: You need to build openssl-1.0.1c first. \033[0m"
		echo ""
		echo "\$ cd ../openssl-1.0.1c/"
		echo "\$ sh xbuild.sh build"
		echo ""
		exit 1
	fi
	
	if [ ! -e ${GPL_PREFIX}/lib/libusb.so ]; then
		echo ""
		echo -e "\033[31m *ERROR*: You need to build libusb-0.1.12 first. \033[0m"
		echo ""
		echo "\$ cd ../libusb-0.1.12/"
		echo "\$ sh xbuild.sh build"
		echo "\$ sh make install"
		echo ""
		exit 1
	fi

	export PATH=${PWD}/../libusb-0.1.12/.install/bin:$PATH
	if [ "$PROJECT_NAME" = "LIGHTNING-4A" ]; then
	CFLAGS="-I${GPL_PREFIX}/include -I${PWD}/../openssl-1.0.1c/xinst/usr/include -I${XINC_DIR}" LDFLAGS="-s -L${GPL_PREFIX}/lib -L${PWD}/../openssl-1.0.1c/xinst/usr/lib -L${PWD}/../zlib-1.2.3 -lssl -lcrypto -lz -L${XLIB_DIR} -lalert -lxml2 -lxmldbc" ./configure --host=${TARGET_HOST} --prefix="/usr/local/ups" --enable-static=no --with-ssl=yes --with-ipv6=no --with-snmp=no --with-usb=yes --with-serial=no --with-user=root --with-group=root --with-hal=no --enable-strip=yes --with-lib=yes --with-ssl-includes="-I${PWD}/../openssl-1.0.1c/xinst/usr/include" --with-ssl-libs="-L${PWD}/../openssl-1.0.1c/xinst/usr/lib" --with-usb-includes="-I${GPL_PREFIX}/include"
	else
	CFLAGS="-I${GPL_PREFIX}/include  -I${XINC_DIR} " LDFLAGS="-s -L${GPL_PREFIX}/lib  -lssl -lcrypto -lz  -L${XLIB_DIR}}" ./configure --host=${TARGET_HOST} --prefix="/usr/local/ups" --enable-static=no --with-ssl=yes --with-ipv6=no --with-snmp=no --with-usb=yes --with-serial=no --with-user=root --with-group=root --with-hal=no --enable-strip=yes --with-lib=yes --with-ssl-includes="-I${XINC_DIR}/openssl-1.0.1c" --with-ssl-libs="-L${XLIB_DIR}" --with-usb-includes="-I${GPL_PREFIX}/include"

	fi
	if [ $? != 0 ]; then
		echo ""
		echo -e "***************************"
		echo -e "configure failed"
		echo ""
		exit 1
	fi

    make
	if [ $? != 0 ]; then
		echo ""
		echo -e "***************************"
		echo -e "make failed"
		echo ""
		exit 1
	fi
	
	if [ "$PROJECT_NAME" = "DNS-340L" ]; then
		$CC -DENABLE_RETRY_GET_USB_DESC -I${PWD}/../libusb-0.1.12 -L${PWD}/../libusb-0.1.12/.libs -lusb -Wall -O2 -s -o upscan upscan.c
		$CC -I${PWD}/../libusb-0.1.12 -L${PWD}/../libusb-0.1.12/.libs -lusb -Wall -O2 -s -o chk_usbdev chk_usbdev.c
	else
		$CC -DENABLE_RETRY_GET_USB_DESC -I${GPL_PREFIX}/include -L${GPL_PREFIX}/lib -lusb -Wall -O2 -s -o upscan upscan.c
		$CC -I${GPL_PREFIX}/include -L${GPL_PREFIX}/lib -lusb -Wall -O2 -s -o chk_usbdev chk_usbdev.c
	fi
    $CC -I${GPL_PREFIX}/include -L${GPL_PREFIX}/lib -lusb -Wall -O2 -s -o prtrscan prtrscan.c
	
	
}

xinstall()
{
   $STRIP -s upscan
   $STRIP -s prtrscan
   
   xcp upscan ${ROOT_FS}/usrsbin
   xcp prtrscan ${ROOT_FS}/usrsbin
   
   if [ "$PROJECT_NAME" = "DNS-340L" ]; then
        xcp chk_usbdev ${ROOT_FS}/usrsbin
   else 
        xcp chk_usbdev ${ROOT_FS}/usrsbin
   fi
   
   $STRIP -s drivers/bcmxcp_usb
   $STRIP -s drivers/blazer_usb
   $STRIP -s drivers/richcomm_usb
   $STRIP -s drivers/tripplite_usb
   $STRIP -s drivers/usbhid-ups
   $STRIP -s drivers/upsdrvctl
   $STRIP -s clients/.libs/upsc
   $STRIP -s clients/.libs/upscmd
   $STRIP -s clients/.libs/upsmon
   $STRIP -s clients/upssched
   $STRIP -s server/upsd
   
   $STRIP -s clients/.libs/libupsclient.so.1
   
   xcp drivers/bcmxcp_usb ${ROOT_FS}/sbin
   xcp drivers/blazer_usb ${ROOT_FS}/sbin
   xcp drivers/richcomm_usb ${ROOT_FS}/sbin
   xcp drivers/tripplite_usb ${ROOT_FS}/sbin
   xcp drivers/usbhid-ups ${ROOT_FS}/sbin
   xcp drivers/upsdrvctl ${ROOT_FS}/sbin
   xcp clients/.libs/upsc ${ROOT_FS}/sbin
   xcp clients/.libs/upscmd ${ROOT_FS}/sbin
   xcp clients/.libs/upsmon ${ROOT_FS}/sbin
   xcp clients/upssched ${ROOT_FS}/sbin
   xcp server/upsd ${ROOT_FS}/sbin
   
   xcp clients/.libs/libupsclient.so.1 ${ROOT_FS}/lib
   
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
