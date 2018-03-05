#!/bin/sh

unset CFLAGS
unset LDFLAGS
unset LIBS

GPL_PREFIX=${PWD}/../_xinstall/${PROJECT_NAME}
mkdir -p ${GPL_PREFIX}/lib

MDADM_VERSION=`basename ${PWD}`

build()
{
	
	make clean
	make
	if [ $? != 0 ]; then
		echo ""
		echo -e "***************************"
		echo -e "make failed"
		echo ""
		exit 1
	fi
	cp -af libmdadm.so ${XLIB_DIR}
	rm -rf ${XINC_DIR}/${MDADM_VERSION}
	mkdir -p ${XINC_DIR}/${MDADM_VERSION}
	cp -af *.h ${XINC_DIR}/${MDADM_VERSION}
}

install()
{
	${CROSS_COMPILE}strip -s libmdadm.so
	${CROSS_COMPILE}strip -s mdadm

	cp -avf mdadm ${ROOT_FS}/bin/
	cp -avf libmdadm.so ${ROOT_FS}/lib/
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
