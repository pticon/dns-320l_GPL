#!/bin/sh

unset CFLAGS
unset LDFLAGS
unset LIBS

source ../xcp.sh
MY_PREFIX=$PWD/../_xinstall/${PROJECT_NAME}

xbuild()
{
	export CPPFLAGS="-I$XINC_DIR/openssl-1.0.1c -I$XINC_DIR -I$MY_PREFIX/include" LDFLAGS="-L$XLIB_DIR -L$MY_PREFIX/lib -lz -lxmldbc -lxml2 -lftp_allow" LIBS="-lpthread"
	#export CPPFLAGS="-I$XINC_DIR/openssl-0.9.7 -I$XINC_DIR" LDFLAGS="-L$XLIB_DIR -lz -lxmldbc -lxml2 -lftp_allow" LIBS="-lpthread"
	if [ "$(has_feature ADS)" = "Yes" ]; then
	
		if [ ! -e ${MY_PREFIX}/lib/libpam.so ]; then
			echo "ERROR: You should build pam first"
			echo ""
			echo "\$ cd ../flex-2.5.4"
			echo "\$ ./xbuild.sh build"
			echo ""
			echo "\$ cd ../Linux-PAM-0.99.7.1"
			echo "\$ ./xbuild.sh build"
			exit 1
		fi
		
		./configure --host=${CC%-*} --with-throttling --with-rfc2640 --with-sysquotas --with-virtualchroot --with-certfile=/etc/ssl/private/pure-ftpd.pem --with-tls --with-pam --without-usernames ac_cv_func_malloc_0_nonnull=yes ac_cv_func_realloc_0_nonnull=yes
		#./configure --host=${CC%-*} --with-throttling --with-rfc2640 --with-sysquotas --with-virtualchroot --with-certfile=/etc/ssl/private/pure-ftpd.pem --with-tls --with-implicittls --with-pam --without-usernames ac_cv_func_malloc_0_nonnull=yes ac_cv_func_realloc_0_nonnull=yes
	
	else
		./configure --host=${CC%-*} --with-throttling --with-rfc2640 --with-sysquotas --with-virtualchroot --with-certfile=/etc/ssl/private/pure-ftpd.pem --with-tls --without-usernames ac_cv_func_malloc_0_nonnull=yes ac_cv_func_realloc_0_nonnull=yes
	fi
	
	make clean
	make
}

xinstall()
{
   ${CROSS_COMPILE}strip src/pure-ftpd -s
   xcp src/pure-ftpd ${ROOT_FS}/bin
}


xclean()
{
   make clean
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
