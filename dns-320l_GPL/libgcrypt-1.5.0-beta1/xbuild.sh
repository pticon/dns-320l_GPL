#!/bin/sh

unset CFLAGS
unset LDFLAGS
unset LIBS

GPL_PREFIX=${PWD}/../_xinstall/${PROJECT_NAME}

build()
{
	mkdir -p ${GPL_PREFIX}

	if [ ! -e ${GPL_PREFIX}/lib/libgpg-error.so ]; then
		echo ""
		echo "*ERROR*: You need to build libgpg-error first."
		echo ""
		echo "\$ cd ../libgpg-error-1.10/"
		echo "\$ sh zbuild.sh build"
		echo ""
		exit 1
	fi

	CFLAGS="$CFLAGS -I${GPL_PREFIX}/include" \
	./configure --host=${TARGET_HOST} --prefix=${GPL_PREFIX} \
		--disable-padlock-support --disable-dependency-tracking --with-pic --enable-noexecstack --disable-O-flag-munging --with-gpg-error-prefix=${GPL_PREFIX}
	if [ $? != 0 ]; then
		echo ""
		echo -e "***************************"
		echo -e "configure failed"
		echo ""
		exit 1
	fi

	make clean
	make
	#if [ $? != 0 ]; then
	#	echo ""
	#	echo -e "***************************"
	#	echo -e "make failed"
	#	echo ""
	#	exit 1
	#fi
	cp -f src/libgcrypt-config ${GPL_PREFIX}/bin	
	cp -af src/gcrypt.h src/gcrypt-module.h ${XINC_DIR}
	cp -af src/gcrypt.h src/gcrypt-module.h ${GPL_PREFIX}/include/
	cp -af src/.libs/libgcrypt.so* ${GPL_PREFIX}/lib/
}

install()
{
	${CROSS_COMPILE}strip -s ${GPL_PREFIX}/lib/libgcrypt.so.11.7.0

	cp -avf ${GPL_PREFIX}/lib/libgcrypt.so* ${ROOT_FS}/lib/
}

clean()
{
	make clean
}

if [ "$1" = "build" ]; then
	build
elif [ "$1" = "install" ]; then
	install
elif [ "$1" = "clean" ]; then
	clean
else
	echo "Usage : $0 build or $0 install or $0 clean"
fi
