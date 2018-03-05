#!/bin/bash

unset CFLAGS
unset LDFLAGS
unset LIBS

source ../xcp.sh

xbuild()
{
	#ac_cv_func_malloc_0_nonnull=yes ac_cv_func_realloc_0_nonnull=yes ./configure --cache-file=config.cache --host=${TARGET_HOST} --with-gnu-ld --prefix=/usr --enable-libdbi --libdir=/lib LDFLAGS="$LDFLAGS -L${XLIB_DIR} -lxmldbc" CFLAGS="$CFLAGS -DALPHA_CUSTOMIZE -I${XINC_DIR}"
	echo ac_cv_func_malloc_0_nonnull=yes > config.cache
   	echo ac_cv_func_realloc_0_nonnull=yes >> config.cache
	./configure --cache-file=config.cache --host=${TARGET_HOST} --with-gnu-ld --prefix=/usr --enable-libdbi --libdir=/lib LDFLAGS="$LDFLAGS -L${XLIB_DIR} -lxmldbc" CFLAGS="$CFLAGS -DALPHA_CUSTOMIZE -I${XINC_DIR}"
	make clean
	make
}
xinstall()
{
	${CROSS_COMPILE}strip -s ./tools/rsyslogd
	xcp ./tools/rsyslogd ${ROOT_FS}/sbin
	cp ./rsyslog.conf.sample ${ROOT_FS}/sbin
	#cp -avf ./rsyslog_dns_340l.conf ${ROOT_FS}/sbin/rsyslog.conf
	cp -avf ./runtime/.libs/lmnsd_ptcp.so 	${ROOT_FS}/lib/rsyslog/lmnsd_ptcp.so
	cp -avf ./runtime/.libs/lmnetstrms.so	${ROOT_FS}/lib/rsyslog/lmnetstrms.so
	cp -avf ./runtime/.libs/lmnet.so		${ROOT_FS}/lib/rsyslog/lmnet.so
	cp -avf ./plugins/imklog/.libs/imklog.so ${ROOT_FS}/lib/rsyslog/imklog.so
	cp -avf ./plugins/imudp/.libs/imudp.so ${ROOT_FS}/lib/rsyslog/imudp.so
	cp -avf ./plugins/imtcp/.libs/imtcp.so ${ROOT_FS}/lib/rsyslog/imtcp.so
	cp -avf ./plugins/omlibdbi/.libs/omlibdbi.so ${ROOT_FS}/lib/rsyslog/omlibdbi.so
	cp -avf ./plugins/imuxsock/.libs/imuxsock.so ${ROOT_FS}/lib/rsyslog/imuxsock.so
	cp -avf ./.libs/lmtcpsrv.so ${ROOT_FS}/lib/rsyslog/lmtcpsrv.so
}

xclean()
{
  make clean ; make distclean
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
