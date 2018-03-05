#!/bin/sh

GPL_PREFIX=${PWD}/../../_xinstall/${PROJECT_NAME}

source ../../xcp.sh

if [ ! -e ${GPL_PREFIX}/lib/libgpg-error.so ]; then
	echo ""
	echo -e "\033[31m *ERROR*: You need to build libgpg-error first. \033[0m"
	echo ""
	echo "\$ cd ../../libgpg-error-1.10/"
	echo "\$ sh xbuild.sh"
	echo ""
	exit 1
fi

if [ ! -e ${GPL_PREFIX}/lib/libgcrypt.so ]; then
	echo ""
	echo -e "\033[31m *ERROR*: You need to build libgcrypt first. \033[0m"
	echo ""
	echo "\$ cd ../../libgcrypt-1.5.0-beta1/"
	echo "\$ sh xbuild.sh"
	echo ""
	exit 1
fi

if [ ! -e ${GPL_PREFIX}/lib/libdb-4.7.so ]; then
	echo ""
	echo -e "\033[31m *ERROR*: You need to build db-4.7.25 first. \033[0m"
	echo ""
	echo "\$ cd ../db-4.7.25/"
	echo "\$ sh build.sh"
	echo ""
	exit 1
fi

if [ ! -e ${GPL_PREFIX}/lib/libpam.so ]; then
	echo ""
	echo -e "\033[31m *ERROR*: You need to build Linux-PAM-0.99.7.1 first. \033[0m"
	echo ""
	echo "\$ cd ../../ADS/Linux-PAM-0.99.7.1/"
	echo "\$ sh xbuild.sh"
	echo ""
	exit 1
fi

xbuild(){

	make clean
	make distclean
# Dirty workaround for pam/*.h inclusion
	if [ "$PROJECT_NAME" != "LIGHTNING-4A" ]; then #LT4A has install to ${GPL_PREFIX}
		mkdir xinst
		cp -r ../../ADS/Linux-PAM-0.99.7.1/_install/* xinst/.
		mkdir -p xinst/include/security
		cp -f xinst/include/*.h xinst/include/security/
	fi


	echo $PROJECT_NAME
	if [ $PROJECT_NAME == "DNS-345" ]; then
		./configure --host=arm-linux --prefix=$PWD/_install --libdir=$PWD/../../lib --with-bdb=$PWD/../db-4.7.25/build_unix/install --enable-cups=no --enable-force-uidgid --enable-afp3 --enable-quota --with-ssl-dir=$PWD/../../openssl-0.9.7 --with-pam=$PWD/../../ADS/Linux-PAM-0.99.7.1/_install/ --with-libgcrypt-dir=$PWD/../../libgcrypt-1.5.0-beta1/xinst --enable-quota=yes --with-ldap=yes --with-acl=yes --with-shadow --with-cnid-cdb-backend --with-cnid-dbd-backend --enable-krbV-uam host_alias=arm-linux CC=${CROSS_COMPILE}gcc CXX=${CROSS_COMPILE}g++

	elif [ "$PROJECT_NAME" == "LIGHTNING-4A" ]; then
	
		./configure --host=arm-linux --exec-prefix=/usr --prefix=/ --libdir=/lib --with-bdb=${GPL_PREFIX} --enable-cups=no --enable-ddp=no --enable-force-uidgid --enable-afp3 --enable-quota --with-ssl-dir=$PWD/../../openssl-0.9.7 --with-pam=${GPL_PREFIX} --with-libgcrypt-dir=${GPL_PREFIX} --enable-quota=yes --with-shadow --with-cnid-cdb-backend --with-cnid-dbd-backend host_alias=arm-linux CC=${CROSS_COMPILE}gcc CXX=${CROSS_COMPILE}g++

	else
  
		./configure --host=arm-linux --libdir=${GPL_PREFIX}/lib --with-bdb=$PWD/../db-4.7.25/build_unix/install --enable-cups=no --enable-ddp=no --enable-force-uidgid --enable-afp3 --enable-quota --with-ssl-dir=$PWD/../../openssl-0.9.7 --with-pam=$PWD/xinst/ --with-libgcrypt-dir=${GPL_PREFIX}/lib --enable-quota=yes --with-shadow --with-cnid-cdb-backend --with-cnid-dbd-backend host_alias=arm-linux CC=${CROSS_COMPILE}gcc CXX=${CROSS_COMPILE}g++
	fi

	make
	#make install

	make install DESTDIR=$PWD/_install
}

xinstall(){
	${CROSS_COMPILE}strip -s $PWD/_install/usr/sbin/afpd
	xcp $PWD/_install/usr/sbin/afpd ${ROOT_FS}/bin
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

#if [ "$PROJECT_NAME" == "LIGHTNING-4A" ]; then
#	make install DESTDIR=${GPL_PREFIX}
#else
#	make install DESTDIR=$PWD/_install
#fi
