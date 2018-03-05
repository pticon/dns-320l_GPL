#!/bin/sh

unset CFLAGS
unset LDFLAGS
unset LIBS

source ../xcp.sh

MY_POSITION=${PWD}
PREFIX=$(readlink -f $PWD/../_xinstall/${PROJECT_NAME})

xbuild()
{	
	if [ ! -e $PREFIX/lib/libz.so.1 ]; then
	cat <<-EOF

	ERROR: "$PREFIX/lib/libz.so.1" does not exist!
	Please build it first.

	EOF

	exit 1
	fi
	
	if [ ! -e $PREFIX/lib/libiconv.so.2 ]; then
	cat <<-EOF

	ERROR: "$PREFIX/lib/libiconv.so.2" does not exist!
	Please build it first.

	EOF

	exit 1
	fi
	
	#build libpng
	cd ${MY_POSITION}/../libpng-1.2.39/
	./xbuild.sh build
	cd $MY_POSITION
	
	#build cryptopp552
	cd ${MY_POSITION}/../cryptopp552/
	./xbuild.sh build
	cd $MY_POSITION
	
	#build wxWidgets
	cd ${MY_POSITION}/../wxWidgets-2.8.9/
	./xbuild.sh build
	cd $MY_POSITION
	
	./configure --host=arm-gnu-linux --enable-amulecmd --enable-ed2k --enable-amule-daemon --enable-webserver --disable-amule-gui --disable-monolithic --with-zlib=$PREFIX/lib --with-wx-config=$PREFIX/bin/wx-config --with-libpng-config=$PREFIX/bin/libpng-config --with-crypto-prefix=$PWD/../cryptopp552 CPPFLAGS="-I$PREFIX/include" LDFLAGS="-L$PREFIX/lib"
	make clean;make CRYPTOPP_LIBS=$PWD/../cryptopp552/libcryptopp.a

}

xinstall()
{
	cd ${MY_POSITION}/../libpng-1.2.39/
	./xbuild.sh install
	cd $MY_POSITION
	
	cd ${MY_POSITION}/../wxWidgets-2.8.9/
	./xbuild.sh install
	cd $MY_POSITION
	
	xcp ./src/amulecmd ${MODULE_DIR}/apkg/addons/${PROJECT_NAME}/aMule/bin/.
	xcp ./src/amuled ${MODULE_DIR}/apkg/addons/${PROJECT_NAME}/aMule/bin/.
	xcp ./src/ed2k ${MODULE_DIR}/apkg/addons/${PROJECT_NAME}/aMule/bin/.
	xcp ./src/webserver/src/amuleweb ${MODULE_DIR}/apkg/addons/${PROJECT_NAME}/aMule/bin/.
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