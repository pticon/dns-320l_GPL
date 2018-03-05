#! /bin/sh
./configure --host=arm-linux --prefix=/opt/firefly --enable-sqlite3  --enable-ssl --enable-iconv \
--with-sqlite3-libs=/opt_arm/uclibc-toolchain-src-20040609/gcc-3.3.x/toolchain_arm/lib \
--with-sqlite3-includes=/opt_arm/uclibc-toolchain-src-20040609/gcc-3.3.x/toolchain_arm/include \
--with-ssl-libs=/opt_arm/uclibc-toolchain-src-20040609/gcc-3.3.x/toolchain_arm/lib \
--with-ssl-includes=/opt_arm/uclibc-toolchain-src-20040609/gcc-3.3.x/toolchain_arm/include
