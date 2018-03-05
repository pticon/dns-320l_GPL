
if [ ! -e "`pwd`/../tmp_install/lib" ]; then
	mkdir -p  `pwd`/../tmp_install/lib
	mkdir  `pwd`/../tmp_install/include
fi

#change work direcotry
#cd `pwd`/../tmp_install
#
#if [ ! -e "`pwd`/../tmp_install/lib/zlib-1.2.3/libz.so.1" ]; then
#	echo "start building zlib"
#
#	[ ! -e "`pwd`/../zlib-1.2.3/libz.so.1" ] && {( cd `pwd`/../zlib-1.2.3 ; sh build.sh;make install prefix=`pwd`/../tmp_install ) || exit 1; }
#
#fi
#
#if [ ! -e "`pwd`/../tmp_install/lib/libssl.so" ]; then
#	echo "start building openssl"
#
#	[ ! -e "`pwd`/../openssl-0.9.7/libssl.so" ] && {( cd `pwd`/../openssl-0.9.7 ; sh build.sh ) || exit 1; }
#
#
#	ln -s `pwd`/../openssl-0.9.7/libssl.so lib/
#	ln -s `pwd`/../openssl-0.9.7/libssl.so.0.9.7 lib/
#	ln -s `pwd`/../openssl-0.9.7/libcrypto.so lib/
#	ln -s `pwd`/../openssl-0.9.7/libcrypto.so.0.9.7 lib/
#
#	cp -a `pwd`/../openssl-0.9.7/include/* include/
#
#fi

(cd ../zlib-1.2.3/ ;sh xbuild.sh build)

if [ ! -e "`pwd`/../tmp_install/lib/libssl.so" ]; then
	echo "start building openssl"

	[ ! -e "`pwd`/../openssl-1.0.1c/libssl.so" ] && {( cd `pwd`/../openssl-1.0.1c ; sh xbuild.sh build) || exit 1; }


#	ln -s `pwd`/../openssl-1.0.1c/libssl.so lib/
#	ln -s `pwd`/../openssl-1.0.1c/libssl.so.1.0.0 lib/
#	ln -s `pwd`/../openssl-1.0.1c/libcrypto.so lib/
#	ln -s `pwd`/../openssl-1.0.1c/libcrypto.so.1.0.0 lib/
#
#	cp -a `pwd`/../openssl-1.0.1c/include/* include/

fi