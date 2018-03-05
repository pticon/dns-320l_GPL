#!/bin/sh
export LDFLAGS="-L/home/momo/works/project/GIT/TWO-BAY-CODEPATH/module/xml_tool/xmldb -lxmldbc"
export CFLAGS=-I/home/momo/works/project/GIT/TWO-BAY-CODEPATH/module/xml_tool/include

#make clean
#make distclean
#make distclean
#../configure --host=arm-none-linux-gnueabi --prefix=/ --with-sntp CFLAGS=-I../../../DNS323-D1/module/include LDFLAGS="-L../../../DNS323-D1/module/lib -s"
#make clean
../configure --host=arm-linux --prefix=/usr --with-sntp --with-openssl-libdir=${PWD}/../../openssl-0.9.7 --with-openssl-incdir=${PWD}/../../openssl-0.9.7/include
#/home/momo/works/project/GIT/TWO-BAY-CODEPATH/module/xml_tool/xmldb
#make
#make DESTDIR=${PWD}/.install
