#!/bin/sh

unset CFLAGS
unset LDFLAGS
unset LIBS

if [ -z "${TOOLCHAIN_LIB}" ] ; then
	echo "You need to export TOOLCHAIN_LIB to correct path."
	echo "COMMAND: \"find \$TOOLCHAIN_PATH/.. -name libgcc_s.so\""
	echo "And then select one of them to do \"export TOOLCHAIN_LIB=\""
	exit 1
fi

GPL_PREFIX=${PWD}/../_xinstall/${PROJECT_NAME}
mkdir -p ${GPL_PREFIX}

build()
{
	if [ ! -e ${GPL_PREFIX}/lib/libdevmapper.so ]; then
		echo ""
		echo "*ERROR*: You need to build LVM first."
		echo ""
		echo "\$ cd ../LVM2.2.02.85/"
		echo "\$ sh zbuild.sh build"
		echo ""
		exit 1
	fi

	if [ ! -e ${GPL_PREFIX}/lib/libgpg-error.so ]; then
		echo ""
		echo "*ERROR*: You need to build libgpg-error first."
		echo ""
		echo "\$ cd ../libgpg-error-1.10/"
		echo "\$ sh zbuild.sh build"
		echo ""
		exit 1
	fi
	
	if [ ! -e ${GPL_PREFIX}/lib/libuuid.so ]; then
		echo ""
		echo "*ERROR*: You need to build e2fsprogs-libs (libuuid) first."
		echo ""
		echo "\$ sh zbuild.sh build"
		echo ""
		exit 1
	fi
	
	if [ ! -e ${GPL_PREFIX}/lib/libpopt.so ]; then
		echo ""
		echo "*ERROR*: You need to build popt first."
		echo ""
		echo "\$ cd ../popt-1.16/"
		echo "\$ sh zbuild.sh build"
		echo ""
		exit 1
	fi

	if [ ! -e ${GPL_PREFIX}/lib/libgcrypt.so ]; then
		echo ""
		echo "*ERROR*: You need to build libgcrypt first."
		echo ""
		echo "\$ cd ../libgcrypt-1.5.0-beta1/"
		echo "\$ sh zbuild.sh build"
		echo ""
		exit 1
	fi

	CFLAGS="$CFLAGS -g -O2 -I${GPL_PREFIX}/include" \
	LDFLAGS="$LDFLAGS -L${GPL_PREFIX}/lib -L${TOOLCHAIN_LIB}" \
	./configure --host=${TARGET_HOST} --prefix=${GPL_PREFIX} \
		--disable-nls --disable-selinux --disable-udev
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
	${CROSS_COMPILE}strip -s ${GPL_PREFIX}/sbin/cryptsetup
	${CROSS_COMPILE}strip -s ${GPL_PREFIX}/lib/libcryptsetup.so.4.4.0

	cp -avf ${GPL_PREFIX}/sbin/cryptsetup ${ROOT_FS}/sbin
	cp -avf ${GPL_PREFIX}/lib/libcryptsetup.so* ${ROOT_FS}/lib/
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

