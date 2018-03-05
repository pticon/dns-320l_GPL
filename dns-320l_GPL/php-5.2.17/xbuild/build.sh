#!/bin/sh
make clean;make distclean

unset CFLAGS
unset LDFLAGS
unset LIBS

XPATH=${PWD}/../../gd_cluster/xinst/bin
export PKG_CONFIG_PATH=${PWD}/../../gd_cluster/xinst/pkgconfig
export PATH=$XPATH:$PATH

export LDFLAGS="-L${PWD}/../../gd_cluster/xinst/lib -L${PWD}/../../openssl-1.0.1c/xinst/usr/lib -lssl -lcrypto -lfontconfig -lxml2 -lz -liconv -lfreetype"
../configure  --host=arm-linux --prefix=${PWD}/.install --without-pear --without-pdo-sqlite --enable-fastcgi --with-config-file-path=/etc/php/ --with-libxml-dir=${PWD}/../../gd_cluster/xinst --with-gd=${PWD}/../../gd_cluster/xinst --with-jpeg-dir=${PWD}/../../gd_cluster/xinst --with-png-dir=${PWD}/../../gd_cluster/xinst --with-zlib-dir=${PWD}/../../gd_cluster/xinst --with-freetype-dir=${PWD}/../../gd_cluster/xinst --with-mysql=${PWD}/../../gd_cluster/libmysql/usr --enable-mbstring --with-curl=${PWD}/../../gd_cluster/xinst --with-curlwrappers --with-ncurses=${PWD}/../../gd_cluster/xinst --with-iconv=${PWD}/../../gd_cluster/xinst --enable-exif --with-openssl=${PWD}/../../openssl-1.0.1c/xinst/usr --with-mcrypt=${PWD}/../../gd_cluster/xinst --with-mhash=${PWD}/../../gd_cluster/xinst --with-curlwrappers

make
