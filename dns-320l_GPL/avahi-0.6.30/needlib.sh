
if [ ! -e "`pwd`/../tmp_install/lib" ]; then
	mkdir -p  `pwd`/../tmp_install/lib
	mkdir  `pwd`/../tmp_install/include
fi

#change work direcotry
cd `pwd`/../tmp_install

if [ ! -e "`pwd`/../tmp_install/lib/libexpat.so.1.5.2" ]; then
	echo "start building expat"

	[ ! -e "`pwd`/../expat-2.0.1/.libs/libexpat.so.1.5.2" ] && {( cd `pwd`/../expat-2.0.1 ; sh build.sh ) || exit 1; }

fi

if [ ! -e "`pwd`/../tmp_install/lib/libdaemon.so.0.5.0" ]; then
	echo "start building libdaemon"

	[ ! -e "`pwd`/../libdaemon-0.14/libdaemon/.libs/libdaemon.so.0.5.0" ] && {( cd `pwd`/../libdaemon-0.14 ; sh build.sh ) || exit 1; }

fi