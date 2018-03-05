#!/bin/sh

MY_PREFIX=$PWD/../_xinstall/${PROJECT_NAME}

build_libogg()
{
	echo "Building libogg-1.2.0"
	cd libogg-1.2.0
	./configure --prefix=$PWD/../xinst --host=arm-linux --disable-dependency-tracking
	make
	if [ ! $? -eq 0 ]; then
		echo ""
		echo "*ERROR*: Build failed in libogg-1.2.0!"
		echo ""
		exit 1
	fi
	make install
	rm -f $PWD/../xinst/lib/*.la
	cd ..
}


build_libvorbis()
{
	echo "Building libvorbis-1.3.1"
	cd libvorbis-1.3.1
	PKG_CONFIG_PATH="$PWD/../xinst/lib/pkgconfig" ./configure --prefix=$PWD/../xinst --host=arm-linux --disable-dependency-tracking
	make
	if [ ! $? -eq 0 ]; then
		echo ""
		echo "*ERROR*: Build failed in libvorbis-1.3.1!"
		echo ""
		exit 1
	fi
	make install
	rm -f $PWD/../xinst/lib/*.la
	cd ..
}

build_libxslt()
{
	echo "Building libxslt-1.1.26"
	if [ ! -e ${MY_PREFIX}/lib/libxml2.so ]; then
		echo "ERROR: You should build libxml2.so"
		exit 1
	fi
	cd libxslt-1.1.26
	patch -p1 < $PWD/../patches/libxslt.m4-libxslt-1.1.26.patch
	patch -p3 < $PWD/../patches/libxslt-1.1.23-parallel-install.patch
	patch -p1 < $PWD/../patches/libxslt-1.1.26-undefined.patch
	./configure --prefix=$PWD/../xinst --host=arm-linux --disable-dependency-tracking --with-python=no --with-crypto=no --with-debug=no --with-debugger=no --with-libxml-include-prefix="$PWD/../../libxml2-2.7.4/include" --with-libxml-libs-prefix="$PWD/../../libxml2-2.7.4/.libs" LDFLAGS="-L$PWD/../../zlib-1.2.3"
	make
	if [ ! $? -eq 0 ]; then
		echo ""
		echo "*ERROR*: Build failed in libxslt-1.1.26!"
		echo ""
		exit 1
	fi
	make install
	rm -f $PWD/../xinst/lib/*.la
	cd ..
}

build_icecast()
{
	echo "Building icecast-2.3.2"
	cd icecast-2.3.2
	patch -p0 < $PWD/../patches/icecast-2.3.2-cross.patch
	#PKG_CONFIG_PATH="$PWD/../xinst/lib/pkgconfig" ./configure --prefix=$PWD/../xinst --host=arm-linux --disable-dependency-tracking XSLTCONFIG="pkg-config libxslt" ac_cv_path_CURL_CONFIG="$PWD/../xcurl-config"
	PATH="$PWD/..:$PATH" ./configure --prefix=$PWD/../xinst --host=arm-linux --disable-dependency-tracking ac_cv_path_XSLTCONFIG="$PWD/../xinst/bin/xslt-config" ac_cv_path_CURL_CONFIG="$PWD/../xcurl-config" PKG_CONFIG_PATH="$PWD/../xinst/lib/pkgconfig" LDFLAGS="-L$PWD/../../zlib-1.2.3"
	make
	if [ ! $? -eq 0 ]; then
		echo ""
		echo "*ERROR*: Build failed in icecast-2.3.2!"
		echo ""
		exit 1
	fi
	make install
	cd ..
}

build_libshout()
{
	echo "Building libshout-2.3.1"
	cd libshout-2.3.1
	PKG_CONFIG_PATH="$PWD/../xinst/lib/pkgconfig" ./configure --prefix=$PWD/../xinst --host=arm-linux --disable-dependency-tracking --disable-theora --disable-speex
	make
	if [ ! $? -eq 0 ]; then
		echo ""
		echo "*ERROR*: Build failed in libshout-2.2.2!"
		echo ""
		exit 1
	fi
	make install
	rm -f $PWD/../xinst/lib/*.la
	cd ..
}

build_lame()
{
	echo "Building lame-3.98.4"
	cd lame-3.98.4
	./configure --prefix=$PWD/../xinst --host=arm-linux --disable-analyzer-hooks --disable-cpml --disable-gtktest
	make
	if [ ! $? -eq 0 ]; then
		echo ""
		echo "*ERROR*: Build failed in lame-3.98.4!"
		echo ""
		exit 1
	fi
	make install
	rm -f $PWD/../xinst/lib/*.la
	cd ..
}

build_ices()
{
	echo "Building ices-0.4"
	cd ices-0.4
	PKG_CONFIG_PATH="$PWD/../xinst/lib/pkgconfig" ./configure --prefix=$PWD/../xinst --host=arm-linux --disable-dependency-tracking --with-xml-config=$PWD/../xml2-config --without-lame --without-vorbis
	make
	if [ ! $? -eq 0 ]; then
		echo ""
		echo "*ERROR*: Build failed in ices-0.4!"
		echo ""
		exit 1
	fi
	make install
	cd ..
}

build_clean()
{
	echo "Clean libogg-1.2.0"
	cd libogg-1.2.0
	make clean
	cd ..
	echo "Clean libvorbis-1.3.1"
	cd libvorbis-1.3.1
	make clean
	cd ..
	echo "Clean libxslt-1.1.26"
	cd libxslt-1.1.26
	make clean
	cd ..
	echo "Clean icecast-2.3.2"
	cd icecast-2.3.2
	make clean
	cd ..
	echo "Clean libshout-2.3.1"
	cd libshout-2.3.1
	make clean
	cd ..
	echo "Clean lame-3.98.4"
	cd lame-3.98.4
	make clean
	cd ..
}

showhelp()
{
	echo ""
	echo "Usage: ./xbuild.sh <libogg|libvorbis|libxslt|icecast|libshout|lame|ices|build|clean>"
	echo ""
}

case "$1" in
	libogg)
		build_libogg
		;;
	libvorbis)
		build_libvorbis
		;;
	libxslt)
		build_libxslt
		;;
	icecast)
		build_icecast
		;;
	libshout)
		build_libshout
		;;
	lame)
		build_lame
		;;
	ices)
		build_ices
		;;
	build)
		build_libogg
		build_libvorbis
		build_libxslt
		build_icecast
		build_libshout
		build_lame
		build_ices
		;;
	clean)
		build_clean	
		;;
	*)
		showhelp
		exit 1
esac

exit 0
