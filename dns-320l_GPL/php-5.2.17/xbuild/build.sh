#!/bin/sh
make clean;make distclean
XPATH=${PWD}/../../gd_cluster/xinst/bin
export PKG_CONFIG_PATH=${PWD}/../../gd_cluster/xinst/pkgconfig
export PATH=$XPATH:$PATH

../configure  --host=arm-linux --prefix=${PWD}/.install --without-pear --without-pdo-sqlite --enable-fastcgi --with-config-file-path=/etc/php/ --with-libxml-dir=${PWD}/../../gd_cluster/xinst --with-gd=${PWD}/../../gd_cluster/xinst --with-jpeg-dir=${PWD}/../../gd_cluster/xinst --with-png-dir=${PWD}/../../gd_cluster/xinst --with-zlib-dir=${PWD}/../../gd_cluster/xinst --with-freetype-dir=${PWD}/../../gd_cluster/xinst --with-mysql=${PWD}/../../gd_cluster/libmysql/usr --enable-mbstring --with-curl=${PWD}/../../gd_cluster/xinst --with-curlwrappers --with-ncurses=${PWD}/../../gd_cluster/xinst --with-iconv=${PWD}/../../gd_cluster/xinst --enable-exif --with-openssl=${PWD}/../../gd_cluster/xinst --with-mcrypt=${PWD}/../../gd_cluster/xinst --with-mhash=${PWD}/../../gd_cluster/xinst --with-curlwrappers

make -j3
