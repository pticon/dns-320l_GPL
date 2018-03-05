unset CFLAGS
unset LDFLAGS
unset LIBS


xbuild()
{
#	sh needlib.sh

	#find ./ * | xargs touch -d `date -d 'today' +%y%m%d`


	cp headfile_tmp/cap_names.h libcap/

	#make
	make LDFLAGS+=" -L. -lattr"

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
	return
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
