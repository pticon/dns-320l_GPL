#!/bin/sh

unset CFLAGS
unset LDFLAGS
unset LIBS

source ../xcp.sh

xbuild()
{
   #sh needlib.sh

   #find ./ * | xargs touch -d `date -d 'today' +%y%m%d`

   ./configure --host=${CC%-*} prefix=`pwd`/tmp_install


   make
   make install STRIP=""
   chmod 755 tmp_install/sbin/*

   ${CC%-*}-strip tmp_install/sbin/*

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
  ${CROSS_COMPILE}strip -s quotacheck
  ${CROSS_COMPILE}strip -s quotaon
  ${CROSS_COMPILE}strip -s repquota
  ${CROSS_COMPILE}strip -s setquota

  xcp quotacheck ${ROOT_FS}/bin
  xcp quotaon ${ROOT_FS}/bin
  xcp repquota ${ROOT_FS}/bin
  xcp setquota ${ROOT_FS}/bin
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
