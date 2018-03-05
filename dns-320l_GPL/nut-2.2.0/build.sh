#!/bin/sh

PREFIX=`basename $CC -gcc`
CXX=${PREFIX}-g++
STRIP=${PREFIX}-strip
LD=${PREFIX}-ld
RANLIB=${PREFIX}-ranlib
AR=${PREFIX}-ar

export PATH=${PWD}/../libusb-0.1.12/.install/bin:$PATH
CFLAGS="-I${PWD}/../libusb-0.1.12 -I${PWD}/../openssl-0.9.7/include" LDFLAGS="-s -L${PWD}/../libusb-0.1.12/.libs -L${PWD}/../openssl-0.9.7" ./configure --host=arm-linux --enable-static=no --with-ssl=yes --with-ipv6=no --with-snmp=no --with-usb=yes --with-serial=no --with-user=root --with-group=root --with-hal=no --enable-strip=yes --with-lib=yes
echo ""
echo "Now, just type \'make\'"
echo ""

make

$CC -I${PWD}/../libusb-0.1.12 -L${PWD}/../libusb-0.1.12/.libs -lusb -Wall -O2 -s -o upscan upscan.c
$CC -I${PWD}/../libusb-0.1.12 -L${PWD}/../libusb-0.1.12/.libs -lusb -Wall -O2 -s -o prtrscan prtrscan.c
