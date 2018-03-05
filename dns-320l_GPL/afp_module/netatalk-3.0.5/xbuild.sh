#!/bin/sh

GPL_PREFIX=${PWD}/../../_xinstall/${PROJECT_NAME}

source ../../xcp.sh

if [ ! -e ${PWD}/../../_xinstall/${PROJECT_NAME}/lib/libgpg-error.so ]; then
	echo ""
	echo -e "\033[31m *ERROR*: You need to build libgpg-error first. \033[0m"
	echo ""
	echo "\$ cd ../../libgpg-error-1.10/"
	echo "\$ sh xbuild.sh"
	echo ""
	exit 1
fi

if [ ! -e ${PWD}/../../_xinstall/${PROJECT_NAME}/lib/libgcrypt.so ]; then
	echo ""
	echo -e "\033[31m *ERROR*: You need to build libgcrypt first. \033[0m"
	echo ""
	echo "\$ cd ../../libgcrypt-1.5.0-beta1/"
	echo "\$ sh xbuild.sh"
	echo ""
	exit 1
fi

if [ ! -e ${PWD}/../../_xinstall/${PROJECT_NAME}/lib/libdb-4.7.so ]; then
	echo ""
	echo -e "\033[31m *ERROR*: You need to build db-4.7.25 first. \033[0m"
	echo ""
	echo "\$ cd ../db-4.7.25/"
	echo "\$ sh build.sh"
	echo ""
	exit 1
fi

if [ ! -e ${PWD}/../../_xinstall/${PROJECT_NAME}/lib/libpam.so ]; then
	echo ""
	echo -e "\033[31m *ERROR*: You need to build Linux-PAM-0.99.7.1 first. \033[0m"
	echo ""
	echo "\$ cd ../../ADS/Linux-PAM-0.99.7.1/"
	echo "\$ sh xbuild.sh"
	echo ""
	exit 1
fi

xbuild()
{
	export CFLAGS="${CFLAGS} -I${GPL_PREFIX}/include"
	export CPPFLAGS="${CFLAGS} -I${GPL_PREFIX}/include"
	export LDFLAGS="${LDFLAGS} -L${GPL_PREFIX}/lib"
	export LIBS="-lrecycle_bin -laatalk"
	
	./configure --host=arm-linux --exec-prefix=/usr --prefix=/ --libdir=/lib --sysconfdir=/etc/netatalk --with-bdb=${GPL_PREFIX} --enable-cups=no --enable-ddp=no --enable-force-uidgid --enable-afp3 --enable-quota --with-ssl-dir=${GPL_PREFIX}/usr --with-pam=${GPL_PREFIX}/ --with-libgcrypt-dir=${GPL_PREFIX} --enable-quota=yes --with-shadow --with-cnid-cdb-backend --with-cnid-dbd-backend host_alias=arm-linux CC=${CROSS_COMPILE}gcc CXX=${CROSS_COMPILE}g++

	if [ $? -eq 0 ]; then
		echo -e "\033[32mconfigure successed!!\033[0m"
		sleep 3
	else
		echo -e "\033[31mconfigure failed!!\033[0m"
		exit 0
	fi

	sed -i '/HAVE_KRB5_FREE_UNPARSED_NAME/a #define HAVE_KRB5_FREE_UNPARSED_NAME 1' $PWD/config.h
	sed -i '/HAVE_KRB5_FREE_ERROR_MESSAGE/a #define HAVE_KRB5_FREE_ERROR_MESSAGE 1' $PWD/config.h


	make clean
	make

	make install DESTDIR=$PWD/_install
}

xinstall()
{
	${CROSS_COMPILE}strip -s $PWD/_install/lib/libatalk.so.6.0.0
	cp -avf $PWD/_install/lib/libatalk.so* ${ROOT_FS}/lib/
	
	${CROSS_COMPILE}strip -s $PWD/_install/lib/netatalk/uams_dhx2_pam.so
	${CROSS_COMPILE}strip -s $PWD/_install/lib/netatalk/uams_dhx2_passwd.so
	${CROSS_COMPILE}strip -s $PWD/_install/lib/netatalk/uams_dhx_pam.so
	${CROSS_COMPILE}strip -s $PWD/_install/lib/netatalk/uams_dhx_passwd.so
	${CROSS_COMPILE}strip -s $PWD/_install/lib/netatalk/uams_guest.so
	${CROSS_COMPILE}strip -s $PWD/_install/lib/netatalk/uams_pam.so
	${CROSS_COMPILE}strip -s $PWD/_install/lib/netatalk/uams_passwd.so
	${CROSS_COMPILE}strip -s $PWD/_install/lib/netatalk/uams_randnum.so
	mkdir -p ${ROOT_FS}/lib/netatalk
	cp -avf $PWD/_install/lib/netatalk/*.so ${ROOT_FS}/lib/netatalk
	
	${CROSS_COMPILE}strip -s $PWD/_install/usr/sbin/afpd
	${CROSS_COMPILE}strip -s $PWD/_install/usr/sbin/cnid_dbd
	${CROSS_COMPILE}strip -s $PWD/_install/usr/sbin/cnid_metad
	${CROSS_COMPILE}strip -s $PWD/_install/usr/sbin/netatalk
	cp -vf $PWD/_install/usr/sbin/* ${ROOT_FS}/sbin
}

xclean()
{
    make clean
    make distclean
}

if [ "$1" = "build" ]; then
    xbuild
elif [ "$1" = "install" ]; then
    xinstall
elif [ "$1" = "clean" ]; then
    xclean
else
    echo "Usage : xbuild.sh {build | install | clean}"
fi
