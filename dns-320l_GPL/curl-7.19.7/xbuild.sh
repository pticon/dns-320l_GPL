#!/bin/sh



case $PROJECT_NAME in

	DNS-327L)
	need_curl_cmd=1;
	;;
	LIGHTNING-4A)
	need_curl_cmd=1;
	;;
	KingsCanyon)
	need_curl_cmd=1;
	;;

    *)
    need_curl_cmd=0;
	;;

esac

unset CFLAGS
unset LDFLAGS
unset LIBS

source ../xcp.sh
MY_PREFIX=$PWD/../_xinstall/${PROJECT_NAME}
xbuild()
{
	#./configure --host=${CC%-*} ac_cv_file___dev_urandom_=yes --with-ssl --with-zlib --prefix='$(shell while [ ! -e "configure" ] ; do cd .. ; done ; echo `pwd` )/../tmp_install/' LDFLAGS="-L`pwd`/../tmp_install/lib" CPPFLAGS="-I`pwd`/../tmp_install/include"
	export CPPFLAGS="-I$MY_PREFIX/include/openssl -I$MY_PREFIX/include" LDFLAGS="-L$MY_PREFIX/lib -lz"
	./configure --host=${CC%-*} ac_cv_file___dev_urandom_=yes --with-ssl --with-zlib --prefix=$(readlink -f $PWD/../_xinstall/${PROJECT_NAME})

	make
	make install
	${CC%-*}-strip ../_xinstall/${PROJECT_NAME}/lib/libcurl.so.4.1.1
	${CC%-*}-strip ../_xinstall/${PROJECT_NAME}/bin/curl
}

xinstall()
{
	${CC%-*}-strip lib/.libs/libcurl.so.4.1.1

	xcp lib/.libs/libcurl.so.4 ${ROOT_FS}/lib
	xcp lib/.libs/libcurl.so ${XLIB_DIR}
	[ $need_curl_cmd -eq 1 ] && xcp ../_xinstall/${PROJECT_NAME}/bin/curl ${ROOT_FS}/bin
}

xclean()
{
	make clean
	git checkout packages tests

	git checkout  Makefile  config.log  config.status  curl-config  docs/Makefile docs/examples/Makefile  docs/libcurl/Makefile
	git checkout  include/Makefile  include/curl/Makefile  include/curl/curlbuild.h  include/curl/stamp-h3  lib/Makefile
	git checkout  lib/curl_config.h  lib/stamp-h1  libcurl.pc libtool src/Makefile src/curl_config.h src/stamp-h2

	rm -rf ../tmp_install/

	rm -rf docs/examples/.deps/
	rm -rf lib/.deps/
	rm -rf src/.deps/
	rm -rf tests/libtest/.deps/
	rm -rf tests/server/.deps/

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
