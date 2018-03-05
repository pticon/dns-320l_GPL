#!/bin/sh

unset CFLAGS
unset LDFLAGS
unset LIBS

source ../xcp.sh

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
	${CROSS_COMPILE}strip -s util/fusermount
	xcp  util/fusermount ${ROOT_FS}/sbin/

	${CROSS_COMPILE}strip -s lib/.libs/libfuse.so.2
	xcp lib/.libs/libfuse.so.2 ${ROOT_FS}/lib
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
