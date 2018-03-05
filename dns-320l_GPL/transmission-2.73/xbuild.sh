#!/bin/sh

source ../xcp.sh

LIBEVENT_PATH=~/unic-gpl/libevent-2.0.19-stable
OPENSSL_PATH=~/unic-gpl/openssl-1.0.1c
CURL_PATH=~/unic-gpl/curl-7.19.7

xbuild()
{
   make clean
   make distclean

	LDFLAGS="-L${XLIB_DIR} -liconv" CFLAGS="${CFLAGS} -I${XINC_DIR}" \
	LIBEVENT_CFLAGS="-I${LIBEVENT_PATH}" LIBEVENT_LIBS="-levent" \
	OPENSSL_CFLAGS="-I${OPENSSL_PATH}" OPENSSL_LIBS="-lssl -lcrypto" \
	LIBCURL_CFLAGS="-I${CURL_PATH}" LIBCURL_LIBS="-lcurl" \
	./configure --host=arm-linux --prefix=${PWD}/xinst
	if [ $? != 0 ] ; then
		echo "configure failed."
		exit 1
	fi


   make
}

xinstall()
{
   make install
   ${CROSS_COMPILE}strip -s ${PWD}/xinst/bin/*
   
   xcp ${PWD}/xinst/bin/transmission-daemon ${ROOT_FS}/usrsbin
   xcp ${PWD}/xinst/bin/transmission-remote ${ROOT_FS}/usrsbin
}

xclean()
{
   make clean
   make distclean
   if [ -d ${PWD}/xinst ]; then
       rm -rf ${PWD}/xinst
   fi
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

