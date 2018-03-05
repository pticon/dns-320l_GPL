#!/bin/bash

xbuild()
{

make clean
make distclean

export PATH=${PWD}/../../_xinstall/${PROJECT_NAME}/bin:$PATH

echo "**********************************************"
echo "*                                            *"
echo "*                                            *"
echo "*   Please make sure the pcre-config have    *"
echo "*   been import correctly!                   *"
echo "*                                            *"
echo "*                                            *"
echo "**********************************************"

../configure \
--host=arm-linux \
--prefix=/usr \
--with-zlib \
--without-bzip2 \
--enable-lfs \
--with-openssl=${PWD}/../../_xinstall/${PROJECT_NAME} \
--without-webdav-props \
--with-pcre=${PWD}/../../_xinstall/${PROJECT_NAME} \
--with-openssl=${PWD}/../../_xinstall/${PROJECT_NAME} \
--with-openssl-includes=${PWD}/../../_xinstall/${PROJECT_NAME}/include \
--with-openssl-libs=${PWD}/../../_xinstall/${PROJECT_NAME}/lib \
CFLAGS="-s -I${PWD}/../../_xinstall/${PROJECT_NAME}/include -I${PWD}/../../_xinstall/${PROJECT_NAME}/usr/include" \
LDFLAGS="-s -L${PWD}/../../_xinstall/${PROJECT_NAME}/lib -L${PWD}/../../_xinstall/${PROJECT_NAME}/usr/lib -lssl -lcrypto -lz"  

make CFLAGS+=-DALPHA_CUSTOMIZE
}

xinstall()
{
	${CROSS_COMPILE}strip -s src/lighttpd $ROOT_FS/sbin/lighttpd
	${CROSS_COMPILE}strip -s src/lighttpd-angel $ROOT_FS/sbin/lighttpd-angel
	${CROSS_COMPILE}strip src/.libs/*.so 

	cp src/.libs/*.so ${ROOT_FS}/web/modules/
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
    echo "Usage : xbuild.sh build or xbuild.sh install or xbuild.sh clean"
 fi
