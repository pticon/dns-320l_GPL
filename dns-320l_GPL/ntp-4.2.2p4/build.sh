#!/bin/bash

./configure --host=arm-linux --prefix=/usr --with-sntp --with-openssl-libdir=${PWD}/../openssl-0.9.7 --with-openssl-incdir=${PWD}/../openssl-0.9.7/include LDFLAGS="-L${PWD}/../lib/ -lxmldbc" CFLAGS="-I${PWD}/../include"
cd sntp
make clean;make