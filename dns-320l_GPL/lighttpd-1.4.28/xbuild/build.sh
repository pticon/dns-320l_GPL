#!/bin/sh
make clean
make distclean

export PATH=${PWD}/../../pcre-7.6/.install/bin:$PATH

echo "**********************************************"
echo "*                                            *"
echo "*                                            *"
echo "*   Please make sure the pcre-config have    *"
echo "*   been import correctly!                   *"
echo "*                                            *"
echo "*                                            *"
echo "**********************************************"

../configure --host=arm-linux --prefix=/usr --with-zlib --without-bzip2 --enable-lfs --with-openssl=${PWD}/../../openssl-0.9.7 --without-webdav-props --with-pcre=${PWD}/../../pcre-7.6/.install CFLAGS="-s -I${PWD}/../../zlib-1.2.3/.install/include -I${PWD}/../../pcre-7.6/.install/include" LDFLAGS="-s -L${PWD}/../../pcre-7.6/.install/lib -L${PWD}/../../zlib-1.2.3/.install/lib"
make CFLAGS+=-DALPHA_CUSTOMIZE

