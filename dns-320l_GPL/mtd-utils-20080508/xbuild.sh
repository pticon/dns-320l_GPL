#!/bin/sh

unset CFLAGS
unset LDFLAGS
unset LIBS

source ../xcp.sh

xbuild()
{
   export CROSS=${CROSS_COMPILE}
   echo $CROSS
   make clean
   make CROSS=${CROSS_COMPILE} WITHOUT_XATTR=1
}

xinstall()
{
   ${CROSS_COMPILE}strip -s $TARGET_HOST/flash_eraseall
   ${CROSS_COMPILE}strip -s $TARGET_HOST/nanddump
   ${CROSS_COMPILE}strip -s $TARGET_HOST/nandfirstgood
   ${CROSS_COMPILE}strip -s $TARGET_HOST/nandwrite
   
   ${CROSS_COMPILE}strip -s ubi-utils/ubiattach
   ${CROSS_COMPILE}strip -s ubi-utils/ubidetach
   ${CROSS_COMPILE}strip -s ubi-utils/ubimkvol
   
   xcp $TARGET_HOST/flash_eraseall ${ROOTDIR}/ramdisk/${PROJECT_NAME}/usr/bin
   xcp $TARGET_HOST/nanddump ${ROOTDIR}/ramdisk/${PROJECT_NAME}/usr/bin
   xcp $TARGET_HOST/nandfirstgood ${ROOTDIR}/ramdisk/${PROJECT_NAME}/usr/bin
   xcp $TARGET_HOST/nandwrite ${ROOTDIR}/ramdisk/${PROJECT_NAME}/usr/bin
   
   xcp ubi-utils/ubiattach ${ROOTDIR}/ramdisk/${PROJECT_NAME}/usr/bin
   xcp ubi-utils/ubidetach ${ROOTDIR}/ramdisk/${PROJECT_NAME}/usr/bin
   xcp ubi-utils/ubimkvol ${ROOTDIR}/ramdisk/${PROJECT_NAME}/usr/bin
}

xclean()
{
	make clean
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


	
