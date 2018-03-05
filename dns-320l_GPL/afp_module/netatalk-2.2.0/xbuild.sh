#!/bin/sh

#GPL_PREFIX=${PWD}/../../_xinstall/${PROJECT_NAME}
GPL_PREFIX=`realpath -s ${PWD}/../../_xinstall/${PROJECT_NAME}`
#if [ ! -e $PWD/../../libgpg-error-1.10/xinst/lib/libgpg-error.so ]; then
#	echo ""
#	echo -e "\033[31m *ERROR*: You need to build libgpg-error first. \033[0m"
#	echo ""
#	echo "\$ cd ../../libgpg-error-1.10/"
#	echo "\$ sh xbuild.sh"
#	echo ""
#	exit 1
#fi
#
#if [ ! -e $PWD/../../libgcrypt-1.5.0-beta1/xinst/lib/libgcrypt.so ]; then
#	echo ""
#	echo -e "\033[31m *ERROR*: You need to build libgcrypt first. \033[0m"
#	echo ""
#	echo "\$ cd ../../libgcrypt-1.5.0-beta1/"
#	echo "\$ sh xbuild.sh"
#	echo ""
#	exit 1
#fi
#
#if [ ! -e $PWD/../db-4.7.25/build_unix/install/lib/libdb-4.7.so ]; then
#	echo ""
#	echo -e "\033[31m *ERROR*: You need to build db-4.7.25 first. \033[0m"
#	echo ""
#	echo "\$ cd ../db-4.7.25/"
#	echo "\$ sh build.sh"
#	echo ""
#	exit 1
#fi
#
#if [ ! -e $PWD/../../ADS/Linux-PAM-0.99.7.1/_install/lib/libpam.so ]; then
#	echo ""
#	echo -e "\033[31m *ERROR*: You need to build Linux-PAM-0.99.7.1 first. \033[0m"
#	echo ""
#	echo "\$ cd ../../ADS/Linux-PAM-0.99.7.1/"
#	echo "\$ sh xbuild.sh"
#	echo ""
#	exit 1
#fi

# Dirty workaround for pam/*.h inclusion
#mkdir xinst
#cp -r ../../ADS/Linux-PAM-0.99.7.1/_install/* xinst/.
#mkdir -p xinst/include/security
#cp -f xinst/include/*.h xinst/include/security/



echo $PROJECT_NAME
if [ $PROJECT_NAME == "DNS-345" ]; then
  ./configure --host=arm-linux --exec-prefix=/usr --sysconfdir=/etc --prefix=/ --libdir=/lib --with-bdb=${GPL_PREFIX} --enable-cups=no --enable-force-uidgid --enable-afp3 --enable-quota --with-ssl-dir=${GPL_PREFIX} --with-pam=${GPL_PREFIX}/ --with-libgcrypt-dir=${GPL_PREFIX} --enable-quota=yes --with-ldap=yes --with-acl=yes --with-shadow --with-cnid-cdb-backend --with-cnid-dbd-backend --enable-krbV-uam host_alias=arm-linux CC=${CROSS_COMPILE}gcc CXX=${CROSS_COMPILE}g++
elif [ "$PROJECT_NAME" == "DNS-340L" ]; then
  ./configure --host=arm-linux --prefix=$PWD/_install --libdir=${GPL_PREFIX}/lib --with-bdb=${GPL_PREFIX} --enable-cups=no --enable-force-uidgid --enable-afp3 --enable-quota --with-ssl-dir=${GPL_PREFIX}/ --with-pam=${GPL_PREFIX}/ --with-libgcrypt-dir=${GPL_PREFIX}/ --enable-quota=yes --with-shadow --with-cnid-cdb-backend --with-cnid-dbd-backend host_alias=arm-linux CC=${CROSS_COMPILE}gcc CXX=${CROSS_COMPILE}g++
elif [ "$PROJECT_NAME" == "DNS-320L"  ]; then
  ./configure --host=arm-linux --exec-prefix=/usr --sysconfdir=/etc --prefix=/ --libdir=/lib  --with-bdb=${GPL_PREFIX} --enable-cups=no --enable-force-uidgid --enable-afp3 --enable-quota --with-ssl-dir=${GPL_PREFIX}/ --with-pam=${GPL_PREFIX}/ --with-libgcrypt-dir=${GPL_PREFIX}/ --enable-quota=yes --with-shadow --with-cnid-cdb-backend --with-cnid-dbd-backend --enable-krbV-uam host_alias=arm-linux CC=${CROSS_COMPILE}gcc CXX=${CROSS_COMPILE}g++
else
  ./configure --host=arm-linux --exec-prefix=/usr --sysconfdir=/etc --prefix=/ --libdir=/lib --with-bdb=${GPL_PREFIX} --enable-cups=no --enable-force-uidgid --enable-afp3 --enable-quota --with-ssl-dir=${GPL_PREFIX}/ --with-pam=${GPL_PREFIX}/ --with-libgcrypt-dir=${GPL_PREFIX}/ --enable-quota=yes --with-shadow --with-cnid-cdb-backend --with-cnid-dbd-backend host_alias=arm-linux CC=${CROSS_COMPILE}gcc CXX=${CROSS_COMPILE}g++
fi

make
#make install
make install DESTDIR=$PWD/_install
