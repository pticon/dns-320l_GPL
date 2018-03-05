#!/bin/sh

if [ ! -e $PWD/../../libgpg-error-1.10/xinst/lib/libgpg-error.so ]; then
	echo ""
	echo -e "\033[31m *ERROR*: You need to build libgpg-error first. \033[0m"
	echo ""
	echo "\$ cd ../../libgpg-error-1.10/"
	echo "\$ sh xbuild.sh"
	echo ""
	exit 1
fi

if [ ! -e $PWD/../../libgcrypt-1.5.0-beta1/xinst/lib/libgcrypt.so ]; then
	echo ""
	echo -e "\033[31m *ERROR*: You need to build libgcrypt first. \033[0m"
	echo ""
	echo "\$ cd ../../libgcrypt-1.5.0-beta1/"
	echo "\$ sh xbuild.sh"
	echo ""
	exit 1
fi

if [ ! -e $PWD/../db-4.7.25/build_unix/install/lib/libdb-4.7.so ]; then
	echo ""
	echo -e "\033[31m *ERROR*: You need to build db-4.7.25 first. \033[0m"
	echo ""
	echo "\$ cd ../db-4.7.25/"
	echo "\$ sh build.sh"
	echo ""
	exit 1
fi

if [ ! -e $PWD/../../ADS/Linux-PAM-0.99.7.1/_install/lib/libpam.so ]; then
	echo ""
	echo -e "\033[31m *ERROR*: You need to build Linux-PAM-0.99.7.1 first. \033[0m"
	echo ""
	echo "\$ cd ../../ADS/Linux-PAM-0.99.7.1/"
	echo "\$ sh xbuild.sh"
	echo ""
	exit 1
fi

./configure --host=arm-linux --prefix=$PWD/_install --libdir=$PWD/../../lib --with-bdb=$PWD/../db-4.7.25/build_unix/install --enable-cups=no --enable-force-uidgid --enable-afp3 --enable-quota --with-ssl-dir=$PWD/../../openssl-0.9.7 --with-pam=$PWD/../../ADS/Linux-PAM-0.99.7.1/_install --with-libgcrypt-dir=$PWD/../../libgcrypt-1.4.6 --enable-quota=yes --with-ldap=yes --with-acl=yes --with-shadow --with-cnid-cdb-backend --with-cnid-dbd-backend --enable-krbV-uam host_alias=arm-linux CC=arm-mv5sft-linux-gnueabi-gcc CXX=arm-mv5sft-linux-gnueabi-g++

make
make install
