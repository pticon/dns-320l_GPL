#!/bin/sh

export PATH=$PATH:${PWD}/../boost-jam-3.1.18/bin.linuxx86/

BOOST_ROOT=${PWD}
echo "BOOST_ROOT=$BOOST_ROOT"
export BOOST_ROOT=${PWD}

BOOST_BUILD_PATH=${PWD}/tools/build/v2
echo "BOOST_BUILD_PATH=$BOOST_BUILD_PATH"
export BOOST_BUILD_PATH=${PWD}/tools/build/v2

echo "***** BUILDING libtorrent 0.15.4 *****"

HOST_PREFIX=arm-mv5sft-linux-gnueabi

./configure --host=$HOST_PREFIX --prefix=${PWD}/../xinst --enable-shared --without-logging \
	--with-dht=on --with-encryption=on --with-zlib=system --with-boost=${PWD}/../xinst \
	--with-asio=shipped --with-openssl=${PWD}/../openssl-0.9.7 \
	LDFLAGS="-lpthread -L${PWD}/../xinst/lib" \
	CPPFLAGS="-I${PWD}/../xinst/include" CFLAGS="-lpthread" CXXFLAGS="-pthread"

make clean;make

