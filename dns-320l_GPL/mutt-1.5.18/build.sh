unset CFLAGS
unset LDFLAGS
unset LIBS

sh needlib.sh

#find ./ * | xargs touch -d `date -d 'today' +%y%m%d`

./configure --host=${CC%-*} --with-libiconv-prefix=`pwd`/../tmp_install/ LDFLAGS=" -L`pwd`/../ncurses-5.5/lib" CPPFLAGS=" -I`pwd`/../ncurses-5.5/include"
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

