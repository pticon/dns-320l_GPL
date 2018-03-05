#!/bin/sh
make clean
make distclean

PREFIX=`basename $CC -gcc`
CXX=${PREFIX}-g++
STRIP=${PREFIX}-strip
LD=${PREFIX}-ld
RANLIB=${PREFIX}-ranlib
AR=${PREFIX}-ar

export PATH=${PWD}/../libusb-0.1.12/.install/bin:$PATH
CFLAGS="-I${XINC_DIR} -I${PWD}/../libusb-0.1.12/.install/include -I${PWD}/../openssl-1.0.1c/xinst/usr/include -DLIGHTNING_4A=1" LDFLAGS="-s -L${PWD}/../libusb-0.1.12/.install/lib -L${PWD}/../openssl-1.0.1c/xinst/usr/lib -L${PWD}/../zlib-1.2.3 -L${XLIB_DIR} -lssl -lcrypto -lz -lalert -lxml2 -lxmldbc" ./configure --host=arm-linux --enable-static=no --with-ssl=yes --with-ipv6=no --with-snmp=no --with-usb=yes --with-serial=no --with-user=root --with-group=root --with-hal=no --enable-strip=yes --with-lib=yes --with-ssl-includes="-I${PWD}/../openssl-1.0.1c/xinst/usr/include" --with-ssl-libs="-L${PWD}/../openssl-1.0.1c/xinst/usr/lib" --with-usb-includes="-I${PWD}/../libusb-0.1.12/.install/include"
echo ""
echo "Now, just type \'make\'"
echo ""

make V=1

$CC -I${PWD}/../libusb-0.1.12 -L${PWD}/../libusb-0.1.12/.libs -lusb -Wall -O2 -s -o upscan upscan.c
$CC -I${PWD}/../libusb-0.1.12 -L${PWD}/../libusb-0.1.12/.libs -lusb -Wall -O2 -s -o prtrscan prtrscan.c
