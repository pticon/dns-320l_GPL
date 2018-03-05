#!/bin/bash

./configure --host=arm-linux --prefix=/usr --with-sntp --with-openssl-libdir=${PWD}/../openssl-0.9.7 --with-openssl-incdir=${PWD}/../openssl-0.9.7/include LDFLAGS=-L${XMLDB}/xml_tool/xmldb -lxmldbc CFLAGS=-I${XMLDB}/xml_tool/include
