
unset CFLAGS
unset LDFLAGS
unset LIBS


source ../xcp.sh

xbuild()
{
 find ./ * | xargs touch -d `date -d 'today' +%y%m%d`

./configure --host=${CC%-*} ac_cv_func_setpgrp_void=yes --prefix='$(shell while [ ! -e "configure" ] ; do cd .. ; done ; echo `pwd` )/../tmp_install/'
make
make install
${CC%-*}-strip ../tmp_install/lib/libdaemon.so.0.5.0


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
	${CC%-*}-strip ../tmp_install/lib/libdaemon.so.0.5.0

	xcp ../tmp_install/lib/libdaemon.so.0 ${ROOT_FS}/lib
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