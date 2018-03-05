#!/bin/sh

unset CFLAGS
unset LDFLAGS
unset LIBS

source ../xcp.sh

xbuild()
{
	sh needlib.sh

	export PATH=$PATH:`pwd`/../cmake-2.8.4-Linux-i386/bin

	make
	#make CFLAGS+=" -s"
	${CC%-*}-strip -s build/genisoimage/genisoimage

	source ./colortab.sh

	 echo -e $fcolor_green$color_bright
	 echo '		##############################################'
	 echo '		# 1.Run "sh clean.sh" to clean all           #'
	 echo '		#   dependancy files                         #'
	 echo '		# 2.Run "sh installfile.sh" to install      #'
	 echo '		#    files to /home/tmp/user                 #'
	 echo '		# 3.Run "sh installfile.sh nas" to install  #'
	 echo '		#    files to nas folder                     #'
	 echo '		##############################################'
	 echo -ne $color_default
}

xinstall()
{
	${CC%-*}-strip -s build/genisoimage/genisoimage

	xcp build/genisoimage/genisoimage ${ROOT_FS}/bin
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