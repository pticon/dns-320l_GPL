#!/bin/sh

unset CFLAGS
unset LDFLAGS
unset LIBS

MY_POSITION=${PWD}
MY_PREFIX=${PWD}/../_xinstall/${PROJECT_NAME}


xbuild()
{
	#build expat
	if [ ! -e $MY_PREFIX/lib/libexpat.so.1 ]; then
		cd ${MY_POSITION}/../expat-2.0.1/
		./configure --host=${TARGET_HOST} --prefix=${MY_PREFIX}
		make 
		make install 
		cd $MY_POSITION
	fi	
	
	if [ ! -e $MY_PREFIX/lib/libdbus-1.so.3 ]; then
		./configure --host=arm-gnu-linux --prefix=${MY_PREFIX} --enable-abstract-sockets \
		--includedir=${MY_PREFIX}/include --libdir=${MY_PREFIX}/lib \
		CPPFLAGS="-I${MY_PREFIX}/include" \
		LDFLAGS="-L${MY_PREFIX}/lib"
    	make all	
    	make install
    	cp dbus/dbus-arch-deps.h ${MY_PREFIX}/include/dbus-1.0/dbus/.
    fi
    
    ./configure --host=arm-gnu-linux --prefix=/ --enable-abstract-sockets \
	--includedir=${MY_PREFIX}/include --libdir=${MY_PREFIX}/lib \
	CPPFLAGS="-I${MY_PREFIX}/include" \
	LDFLAGS="-L${MY_PREFIX}/lib"
    make all	
    	
}

xinstall()
{	
	$STRIP ${PWD}/dbus/.libs/libdbus-1.so.3
	$STRIP ${PWD}/bus/dbus-daemon
	$STRIP ${PWD}/tools/.libs/dbus-uuidgen

	cp ${PWD}/dbus/.libs/libdbus-1.so.3 ${ROOT_FS}/lib/libdbus-1.so.3
	cp ${PWD}/bus/dbus-daemon ${ROOT_FS}/bin/dbus-daemon
	cp ${PWD}/tools/.libs/dbus-uuidgen ${ROOT_FS}/bin/dbus-uuidgen
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
   echo "Usage : xbuild.sh build or xbuild.sh install"
fi