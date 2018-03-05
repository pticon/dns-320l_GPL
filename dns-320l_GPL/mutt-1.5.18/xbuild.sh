unset CFLAGS
unset LDFLAGS
unset LIBS

source ../xcp.sh

xbuild()
{

sh needlib.sh

#find ./ * | xargs touch -d `date -d 'today' +%y%m%d`

./configure --host=${CC%-*} --with-libiconv-prefix=`pwd`/../tmp_install LDFLAGS=" -L`pwd`/../ADS/ncurses-5.7/lib" CPPFLAGS=" -I`pwd`/../ADS/ncurses-5.7/include"
make
make install prefix=`pwd`/tmp_install
${CC%-*}-strip tmp_install/bin/mutt


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
	${CC%-*}-strip tmp_install/bin/mutt

	xcp tmp_install/bin/mutt ${ROOT_FS}/bin
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
