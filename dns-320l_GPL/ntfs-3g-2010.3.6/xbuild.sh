#!/bin/sh

unset CFLAGS
unset LDFLAGS
unset LIBS

GPL_PREFIX=${PWD}/../_xinstall/${PROJECT_NAME}
mkdir -p ${GPL_PREFIX}

build()
{
	./configure --host=${TARGET_HOST} --prefix=${GPL_PREFIX}
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
}

install()
{
	${CROSS_COMPILE}strip -s libntfs-3g/.libs/libntfs-3g.so.75.0.0
	${CROSS_COMPILE}strip -s src/.libs/ntfs-3g
   
	cp -avf libntfs-3g/.libs/libntfs-3g.so* ${ROOT_FS}/lib
	cp -avf src/.libs/ntfs-3g ${ROOT_FS}/sbin
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
