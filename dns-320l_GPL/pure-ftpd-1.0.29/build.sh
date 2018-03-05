#!/bin/sh
unset CFLAGS
unset LDFLAGS
unset LIBS

cd ../lib/
LIB_PATH=`pwd`
echo $LIB_PATH
cd ../include
INCLUDE_PATH=`pwd`
echo $INCLUDE_PATH
cd ../pure-ftpd-1.0.29

case ${PROJECT_NAME} in
	DNS-320L)
		export CPPFLAGS=-I$INCLUDE_PATH LDFLAGS="-L$LIB_PATH -lxmldbc -lxml2 -lftp_allow" LIBS="-lpthread"
		./configure --host=arm-mv5sft-linux-gnueabi --with-throttling --with-rfc2640 --with-sysquotas --with-virtualchroot --with-certfile=/etc/ssl/private/pure-ftpd.pem --with-tls --without-usernames ac_cv_func_malloc_0_nonnull=yes ac_cv_func_realloc_0_nonnull=yes
		;;

#	modify by andrew 20120710_start
	DNS-320B)
		export CPPFLAGS=-I$INCLUDE_PATH LDFLAGS="-L$LIB_PATH -lxmldbc -lxml2 -lftp_allow" LIBS="-lpthread"
		./configure --host=arm-mv5sft-linux-gnueabi --with-throttling --with-rfc2640 --with-sysquotas --with-virtualchroot --with-certfile=/etc/ssl/private/pure-ftpd.pem --with-tls --without-usernames ac_cv_func_malloc_0_nonnull=yes ac_cv_func_realloc_0_nonnull=yes
		;;
#	modify by andrew 20120710_end
	
#	modify by andrew 20120724_start
	DNS-327L)
		export CPPFLAGS=-I$INCLUDE_PATH LDFLAGS="-L$LIB_PATH -lxmldbc -lxml2 -lftp_allow" LIBS="-lpthread"
		./configure --host=arm-mv5sft-linux-gnueabi --with-throttling --with-rfc2640 --with-sysquotas --with-virtualchroot --with-certfile=/etc/ssl/private/pure-ftpd.pem --with-tls --without-usernames ac_cv_func_malloc_0_nonnull=yes ac_cv_func_realloc_0_nonnull=yes
		;;
#	modify by andrew 20120724_end
	
#   modify by andrew start 2012.04.17
#	DNS-345)
#		./configure --host=arm-mv5sft-linux-gnueabi --with-throttling --with-rfc2640 --with-sysquotas --with-virtualchroot #--with-certfile=/etc/ssl/private/pure-ftpd.pem --with-tls --with-pam --without-usernames CPPFLAGS=-I$INCLUDE_PATH LDFLAGS=-L$LIB_PATH #LIBS="-lpthread" ac_cv_func_malloc_0_nonnull=yes ac_cv_func_realloc_0_nonnull=yes
#		;;
	
	DNS-345)
		export CPPFLAGS=-I$INCLUDE_PATH LDFLAGS="-L$LIB_PATH -lxmldbc -lxml2 -lftp_allow" LIBS="-lpthread"
		./configure --host=arm-mv5sft-linux-gnueabi --with-throttling --with-rfc2640 --with-sysquotas --with-virtualchroot --with-certfile=/etc/ssl/private/pure-ftpd.pem --with-tls --with-pam --without-usernames ac_cv_func_malloc_0_nonnull=yes ac_cv_func_realloc_0_nonnull=yes
		;;
#   modify by andrew end 2012.04.17
esac		

make clean;make
