#!/bin/bash

unset CFLAGS
unset LDFLAGS
unset LIBS

GPL_PREFIX=${PWD}/../_xinstall/${PROJECT_NAME}

build()
{
	mkdir -p ${GPL_PREFIX}

	./configure --host=${TARGET_HOST} --prefix=${GPL_PREFIX} \
		--disable-nls --disable-languages
	if [ $? != 0 ]; then
		echo ""
		echo -e "***************************"
		echo -e "configure failed"
		echo ""
		exit 1
	fi

	make clean
	make
	if [ $? != 0 ]; then
		echo ""
		echo -e "***************************"
		echo -e "make failed"
		echo ""
		exit 1
	fi
	
	make install
	cp -af src/.libs/libgpg-error.so* ${XLIB_DIR}
	cp -af src/gpg-error.h ${XINC_DIR}
}

install()
{
	${CROSS_COMPILE}strip ${GPL_PREFIX}/lib/libgpg-error.so.0.8.0
	
	cp -avf ${GPL_PREFIX}/lib/libgpg-error.so* ${ROOT_FS}/lib/
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
