
if [ ! -e "`pwd`/../tmp_install/lib" ]; then
	mkdir -p  `pwd`/../tmp_install/lib
	mkdir  `pwd`/../tmp_install/include
fi

#change work direcotry
cd `pwd`/../tmp_install

if [ ! -e "`pwd`/../tmp_install/lib/libssl.so" ]; then
	echo "start building openssl"

	[ ! -e "`pwd`/../openssl-0.9.7/libssl.so" ] && {( cd `pwd`/../openssl-0.9.7 ; sh build.sh ) || exit 1; }


	ln -s `pwd`/../openssl-0.9.7/libssl.so lib/
	ln -s `pwd`/../openssl-0.9.7/libssl.so.0.9.7 lib/
	ln -s `pwd`/../openssl-0.9.7/libcrypto.so lib/
	ln -s `pwd`/../openssl-0.9.7/libcrypto.so.0.9.7 lib/

	cp -a `pwd`/../openssl-0.9.7/include/* include/

fi
