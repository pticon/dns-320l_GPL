#!/bin/sh

unset CFLAGS
unset LDFLAGS
unset LIBS

source ../../xcp.sh

xbuild()
{
	echo samba_cv_CC_NEGATIVE_ENUM_VALUES=yes > linux.cache
	echo ac_cv_header_stdc=yes >> linux.cache
	echo ac_cv_c_bigendian=no >> linux.cache
	echo libreplace_cv_READDIR_GETDENTS=no >> linux.cache
	echo samba_cv_HAVE_WRFILE_KEYTAB=no >> linux.cache
	echo smb_krb5_cv_enctype_to_string_takes_krb5_context_arg=no >> linux.cache
	echo ac_cv_func_memcmp_working=yes >> linux.cache
	echo libreplace_cv_HAVE_SECURE_MKSTEMP=yes >> linux.cache
	echo libreplace_cv_HAVE_C99_VSNPRINTF=yes >> linux.cache
	echo libreplace_cv_READDIR_GETDIRENTRIES=no >> linux.cache
	echo samba_cv_HAVE_EXPLICIT_LARGEFILE_SUPPORT=yes >> linux.cache
	echo samba_cv_have_longlong=yes >> linux.cache
	echo ac_cv_lib_ext_z_zlibVersion=yes >> linux.cache
	echo ac_cv_header_zlib_h=yes >> linux.cache
	echo ac_cv_lib_ext_z=yes >> linux.cache
	echo samba_cv_HAVE_NATIVE_ICONV=yes >> linux.cache
	echo samba_cv_HAVE_OFF64_T=yes >> linux.cache
	echo samba_cv_HAVE_STRUCT_FLOCK64=yes >> linux.cache
	echo samba_cv_USE_SETRESUID=yes >> linux.cache

   ./configure --build=i386-linux --host=${TARGET_HOST} --cache-file=linux.cache --with-privatedir=/etc/samba --with-lockdir=/tmp/samba --with-configdir=/etc/samba --libdir=/lib --with-piddir=/var/run/samba --with-logfilebase=/var/log/samba --enable-cups=no --prefix=/etc/samba --enable-largefile --with-sendfile-support=yes --with-quotas=yes --with-acl-support=yes --enable-shared=yes --without-ads --without-winbind --without-pam --without-ldap --without-libsmbclient --with-libiconv=$PWD/../../_xinstall/${PROJECT_NAME}/

#   cp include/config.roy include/config.h
   make libtalloc
   make libtdb
   make bin/smbd
   make bin/nmbd
   make bin/smbpasswd
   make bin/smbclient
   make bin/nmblookup
   make bin/recycle.so
   make bin/streams_xattr.so
   ${CC} -o bin/getent $PWD/../testsuite/nsswitch/getent.c
}

xinstall()
{
   ${CROSS_COMPILE}strip -s bin/smbd
   ${CROSS_COMPILE}strip -s bin/nmbd
   ${CROSS_COMPILE}strip -s bin/smbpasswd
   ${CROSS_COMPILE}strip -s bin/smbclient
   ${CROSS_COMPILE}strip -s bin/nmblookup
   ${CROSS_COMPILE}strip -s bin/recycle.so
   ${CROSS_COMPILE}strip -s bin/streams_xattr.so
   ${CROSS_COMPILE}strip -s bin/getent
   
   cp bin/smbd ${ROOT_FS}/bin
   cp bin/nmbd ${ROOT_FS}/bin
   cp bin/nmblookup ${ROOT_FS}/bin
   cp bin/smbclient ${ROOT_FS}/bin
   cp bin/smbpasswd ${ROOT_FS}/bin
   cp bin/getent ${ROOT_FS}/bin
   cp bin/recycle.so ${ROOT_FS}/lib/vfs
   cp bin/streams_xattr.so ${ROOT_FS}/lib/vfs
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
   echo "Usage : [xbuild.sh build] or [xbuild.sh install] or [xbuild.sh clean]"
fi

