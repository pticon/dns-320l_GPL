#! /bin/sh

#./configure --host=arm-linux --prefix=${PWD}/.install LD=$(CC) --with-zlib=${PWD}/../zlib-1.2.3/_install/
#make


unset CFLAGS
unset LDFLAGS
unset LIBS

sh needlib.sh

#find ./ * | xargs touch -d `date -d 'today' +%y%m%d`


.#/configure --host=${CC%-*}  LD=$(CC) --prefix= --with-zlib=${PWD}/../zlib-1.2.3 --with-ssl-dir=`pwd`/../tmp_install/
./configure --host=${CC%-*}  LD=$(CC) --prefix= --with-zlib=${PWD}/../tmp_install --with-ssl-dir=`pwd`/../tmp_install/

make
make install-files STRIP_OPT= DESTDIR=`pwd`/tmp_install

${CC%-*}-strip tmp_install/sbin/sshd
${CC%-*}-strip tmp_install/bin/ssh
${CC%-*}-strip tmp_install/bin/ssh-keygen

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
