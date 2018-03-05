#!/bin/bash

source ../../xcp.sh

xbuild()
{
	make clean
	make distclean


	XPATH=${PWD}/../../_xinstall/${PROJECT_NAME}
	export PKG_CONFIG_PATH=${XPATH}/pkgconfig
	export PATH=$XPATH/bin:$PATH

	export LDFLAGS="${LDFLAGS} -L${XPATH}/lib -L${XPATH}/usr/lib -lssl -lcrypto -lfontconfig -lxml2 -lz -liconv -lfreetype"

	../configure  \
	--host=${TARGET_HOST} \
	--prefix=${PWD}/.install \
	--without-pear \
	--with-curlwrappers \
	--without-pdo-sqlite \
	--enable-exif \
	--enable-fastcgi \
	--enable-mbstring \
	--with-curlwrappers \
	--with-config-file-path=/etc/php/ \
	--with-libxml-dir=${XPATH} \
	--with-gd=${XPATH} \
	--with-jpeg-dir=${XPATH} \
	--with-png-dir=${XPATH} \
	--with-zlib-dir=${XPATH} \
	--with-freetype-dir=${XPATH} \
	--with-mysql=${XPATH} \
	--with-curl=${XPATH} \
	--with-ncurses=${XPATH} \
	--with-iconv=${XPATH} \
	--with-openssl=${XPATH}/usr \
	--with-mcrypt=${XPATH} \
	--with-mhash=${XPATH} \
	--with-libxml-dir=${XPATH} \
	--with-mysql-sock=/tmp/mysql.sock \
	--with-mysql=${XPATH}

	#ac_cv_php_xml2_config_path="${XPATH}/bin/xml2-config"

	make
}

xinstall()
{
	${CROSS_COMPILE}strip -s ./sapi/cli/php
	${CROSS_COMPILE}strip -s ./sapi/cgi/php-cgi

	xcp ./sapi/cli/php ${ROOT_FS}/bin/php
	xcp ./sapi/cgi/php-cgi ${ROOT_FS}/bin/php-cgi
}

xclean()
{
	make clean
	make distclean
}

if [ "$1" = "build" ]; then
	xbuild
elif [ "$1" = "install" ]; then
	xinstall
elif [ "$1" = "clean" ]; then
	xclean
else
  echo "Usage : xbuild.sh build or xbuild.sh install or xbuild.sh clean"
fi

