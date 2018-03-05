#!/bin/bash

unset CFLAGS
unset LDFLAGS
unset LIBS


source ../xcp.sh

xbuild()
{
./configure --host=${CC%-*} --prefix='$(shell while [ ! -e "configure" ] ; do cd .. ; done ; echo `pwd` )/../_xinstall/${PROJECT_NAME}'
make buildlib
make installlib
${CC%-*}-strip ../_xinstall/${PROJECT_NAME}/lib/libexpat.so.1.5.2

source ./colortab.sh

echo -e $fcolor_green$color_bright
echo '		##############################################'
echo '		# 1.Run "sh clean.sh" to clean all           #'
echo '		#   dependancy files                         #'
echo '		# 2.Run "sh .installfile.sh" to install      #'
echo '		#    files to /home/tmp/user                 #'
echo '		# 3.Run "sh .installfile.sh nas" to install  #'
echo '		#    files to nas folder                     #'
echo '		##############################################'
echo -ne $color_default

}

xinstall()
{
	${CC%-*}-strip ../_xinstall/${PROJECT_NAME}/lib/libexpat.so.1.5.2

	xcp ../_xinstall/${PROJECT_NAME}/lib/libexpat.so.1 ${ROOT_FS}/lib
	xcp ../_xinstall/${PROJECT_NAME}/lib/libexpat.so.1 ${XLIB_DIR}/libexpat.so
}

xclean()
{
	sh clean.sh
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
