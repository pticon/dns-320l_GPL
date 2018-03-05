#!/bin/sh

unset CFLAGS
unset LDFLAGS
unset LIBS

source ../xcp.sh

xbuild()
{
   #./configure --host=${TARGET_HOST} --without-selinux --enable-shared=yes
   ./configure --host=${TARGET_HOST} --without-selinux --enable-shared=yes --without-libpam
   make clean
   make
}

xinstall()
{
   ${CROSS_COMPILE}strip -s lib/.libs/libshadow.so.0
   ${CROSS_COMPILE}strip -s src/.libs/usermod
   ${CROSS_COMPILE}strip -s src/.libs/userdel
   ${CROSS_COMPILE}strip -s src/.libs/gpasswd
   ${CROSS_COMPILE}strip -s src/.libs/groupmod

   xcp lib/.libs/libshadow.so.0 ${ROOT_FS}/lib
   xcp src/.libs/usermod ${ROOT_FS}/bin
   xcp src/.libs/userdel ${ROOT_FS}/bin
   xcp src/.libs/gpasswd ${ROOT_FS}/bin
   xcp src/.libs/newgrp ${ROOT_FS}/bin
   if [ "$(has_feature CUSTOM_WD)" = "Yes" ]; then
	  ${CROSS_COMPILE}strip -s src/.libs/chfn
      ${CROSS_COMPILE}strip -s src/.libs/newgrp
      xcp src/.libs/chfn ${ROOT_FS}/bin
      xcp src/.libs/newgrp ${ROOT_FS}/bin
	  xcp src/.libs/groupmod ${ROOT_FS}/bin
   fi
}

xclean()
{
   if [ -e Makefile ]; then
   	make clean
   else
   	echo "can not find Makefile"
   fi
}

if [ "$1" = "build" ]; then
   xbuild
elif [ "$1" = "install" ]; then
   xinstall
elif [ "$1" = "clean" ]; then
   xclean
else
   echo "Usage : xbuild.sh build or xbuild.sh install or xbuild.sh clean"
fi
