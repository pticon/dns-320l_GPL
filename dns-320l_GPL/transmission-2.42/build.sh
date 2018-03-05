#!/bin/sh

TARGET_HOST=arm-mv5sft-linux-gnueabi
TOOLCHAIN_FS=/opt_gccarm/arm-mv5sft-linux-gnueabi/arm-mv5sft-linux-gnueabi/sys-root/usr

LIBEVENT_PATH=../libevent-2.0.16-stable
OPENSSL_PATH=../openssl-0.9.7
CURL_PATH=../curl-7.19.7

if [ ! -e ${TOOLCHAIN_FS}/lib/libevent.so ]; then
	echo ""
	echo "*ERROR*: You need to build libevent first."
	echo ""
	echo "\$ cd ${LIBEVENT_PATH}"
	echo "\$ sh build.sh"
	echo ""
	exit 1
fi
if [ ! -e ${TOOLCHAIN_FS}/lib/libssl.so ]; then
	echo ""
	echo "*ERROR*: You need to build libssl first."
	echo ""
	echo "\$ cd ${OPENSSL_PATH}"
	echo "\$ sh build.sh"
	echo ""
	exit 1
fi
if [ ! -e ${TOOLCHAIN_FS}/lib/libcurl.so ]; then
	echo ""
	echo "*ERROR*: You need to build curl first."
	echo ""
	echo "\$ cd ${CURL_PATH}"
	echo "\$ sh build.sh"
	echo ""
	exit 1
fi

LDFLAGS="-L${TOOLCHAIN_FS}/lib -liconv" CFLAGS="-I${TOOLCHAIN_FS}/include" \
LIBEVENT_CFLAGS="-I${LIBEVENT_PATH}" LIBEVENT_LIBS="-levent" \
OPENSSL_CFLAGS="-I${OPENSSL_PATH}" OPENSSL_LIBS="-lssl -lcrypto" \
LIBCURL_CFLAGS="-I${CURL_PATH}" LIBCURL_LIBS="-lcurl" \
CFLAGS="-DALPHA_CUSTOMIZE" \
./configure --host=${TARGET_HOST} --prefix=${TOOLCHAIN_FS} --disable-mac
if [ $? != 0 ] ; then
	echo "configure failed."
	exit 1
fi

make clean
make
if [ $? != 0 ] ; then
	echo "make failed."
	exit 1
fi

make -C web install
