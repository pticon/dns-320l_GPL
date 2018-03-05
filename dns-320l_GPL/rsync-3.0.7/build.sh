#! /bin/sh

./configure.sh --host=arm-mv5sft-linux-gnueabi --enable-maintainer-mode --disable-ipv6
make clean;make
