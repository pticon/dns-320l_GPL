#!/bin/sh

unset CFLAGS
unset LDFLAGS
unset LIBS

GPL_PREFIX=${PWD}/../_xinstall/${PROJECT_NAME}
mkdir -p ${GPL_PREFIX}

build()
{
	CFLAGS="$CFLAGS -g -O2" \
	./configure --host=${TARGET_HOST} --prefix=${GPL_PREFIX} \
		--disable-testio-debug --disable-tls --enable-elf-shlibs
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
	mkdir -p ${GPL_PREFIX}/include/uuid
	cp -af lib/uuid/*.h ${GPL_PREFIX}/include/uuid/
}

install()
{
	${STRIP} ${GPL_PREFIX}/sbin/badblocks
	${STRIP} ${GPL_PREFIX}/sbin/blkid
	${STRIP} ${GPL_PREFIX}/sbin/dumpe2fs
	${STRIP} ${GPL_PREFIX}/sbin/e2fsck
	${STRIP} ${GPL_PREFIX}/sbin/mke2fs
	${STRIP} ${GPL_PREFIX}/sbin/resize2fs
	${STRIP} ${GPL_PREFIX}/sbin/tune2fs

	${STRIP} ${GPL_PREFIX}/lib/libblkid.so.1.0
	${STRIP} ${GPL_PREFIX}/lib/libuuid.so.1.2
	${STRIP} ${GPL_PREFIX}/lib/libcom_err.so.2.1
	${STRIP} ${GPL_PREFIX}/lib/libe2p.so.2.3
	${STRIP} ${GPL_PREFIX}/lib/libext2fs.so.2.4
	${STRIP} ${GPL_PREFIX}/lib/libss.so.2.0

	mkdir -p ${ROOT_FS}/bin
	cp -avf ${GPL_PREFIX}/sbin/badblocks ${GPL_PREFIX}/sbin/blkid ${GPL_PREFIX}/sbin/dumpe2fs ${GPL_PREFIX}/sbin/e2fsck ${GPL_PREFIX}/sbin/mke2fs ${GPL_PREFIX}/sbin/resize2fs ${GPL_PREFIX}/sbin/tune2fs ${ROOT_FS}/bin
	mkdir -p ${ROOT_FS}/lib
	cp -avf ${GPL_PREFIX}/lib/libblkid.so* ${GPL_PREFIX}/lib/libuuid.so* ${GPL_PREFIX}/lib/libcom_err.so* ${GPL_PREFIX}/lib/libe2p.so* ${GPL_PREFIX}/lib/libext2fs.so* ${GPL_PREFIX}/lib/libss.so* ${ROOT_FS}/lib
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
