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
	
	make install
}

install()
{
   ${CROSS_COMPILE}strip -s ${GPL_PREFIX}/bin/sg_scan
   ${CROSS_COMPILE}strip -s ${GPL_PREFIX}/bin/sg_inq
   
   ${CROSS_COMPILE}strip -s ${GPL_PREFIX}/lib/libsgutils2.so.2.0.0
   
   cp -avf ${GPL_PREFIX}/bin/sg_scan ${GPL_PREFIX}/bin/sg_inq ${ROOT_FS}/bin/
   cp -avf ${GPL_PREFIX}/lib/libsgutils2.so* ${GPL_PREFIX}/bin/sg_inq ${ROOT_FS}/lib/
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
