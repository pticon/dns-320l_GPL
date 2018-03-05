#! /bin/sh

./configure --build=i386-linux --host=arm-mv5sft-linux-gnueabi --cache-file=linux.cache --with-privatedir=/etc/samba --with-lockdir=/tmp/samba --with-configdir=/etc/samba --libdir=/lib --with-piddir=/var/run/samba --with-logfilebase=/var/log/samba --enable-cups=no --prefix=/etc/samba --enable-largefile=yes --with-sendfile-support=yes --with-quotas=yes --disable-static --without-fhs --with-ads=yes --with-ldap=yes --with-winbind=yes --with-acl-support=yes --without-libaddns --without-libsmbclient --with-included-popt --with-included-iniparser --with-pam=$PWD/../../ADS/Linux-PAM-0.99.7.1/_install --with-krb5=$PWD/../../ADS/krb5-1.7.1/src/_install --with-libiconv=$PWD/../../libiconv-1.9.2/_install
cp include/config.roy include/config.h
make clean
make libtalloc;make libtdb;make bin/smbd;make bin/nmbd
make bin/smbpasswd;make bin/smbclient;make bin/nmblookup
make pam_winbind; make nss_modules
make bin/wbinfo; make bin/winbindd

