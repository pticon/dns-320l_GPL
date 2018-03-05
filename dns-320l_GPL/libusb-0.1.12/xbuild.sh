#!/bin/sh

unset CFLAGS
unset LDFLAGS
unset LIBS

GPL_PREFIX=${PWD}/../_xinstall/${PROJECT_NAME}
mkdir -p ${GPL_PREFIX}

build()
{
	make distclean

	./configure --host=${TARGET_HOST} --prefix=${GPL_PREFIX}
	if [ $? != 0 ]; then
		echo ""
		echo -e "***************************"
		echo -e "configure failed"
		echo ""
		exit 1
	fi
	
	make
	if [ $? != 0 ]; then
		echo ""
		echo -e "***************************"
		echo -e "make failed"
		echo ""
		exit 1
	fi
	
	make install
	cp -af .libs/libusb-0.1.so.4.4.4 .libs/libusb-0.1.so.4 .libs/libusb.so ${GPL_PREFIX}/lib
}

install()
{
	${CROSS_COMPILE}strip ${GPL_PREFIX}/lib/libusb-0.1.so.4.4.4
	
	cp -avf ${GPL_PREFIX}/lib/libusb-0.1.so.4.4.4 ${ROOT_FS}/lib/
	ln -sf libusb-0.1.so.4.4.4 ${ROOT_FS}/lib/libusb-0.1.so.4
	ln -sf libusb-0.1.so.4.4.4 ${ROOT_FS}/lib/libusb.so
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
