
if [ ! -e "`pwd`/../tmp_install/lib" ]; then
	mkdir -p  `pwd`/../tmp_install/lib
	mkdir  `pwd`/../tmp_install/include
fi

#change work direcotry
cd `pwd`/../tmp_install

if [ ! -e "`pwd`/../tmp_install/lib/libiconv.so" ]; then
	echo "start building lib"

	[ ! -e "`pwd`/../libiconv-1.9.2/lib/.libs/libiconv.so.2" ] && {( cd `pwd`/../libiconv-1.9.2 ; sh build.sh ) || exit 1; }


	ln -s `pwd`/../libiconv-1.9.2/lib/.libs/libiconv.so.2 lib/
	ln -s `pwd`/../libiconv-1.9.2/lib/.libs/libiconv.so lib/


	cp -a `pwd`/../libiconv-1.9.2/include/* include/

fi
