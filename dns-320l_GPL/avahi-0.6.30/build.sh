unset CFLAGS
unset LDFLAGS
unset LIBS

sh needlib.sh

find ./ * | xargs touch -d `date -d 'today' +%y%m%d`


[ ! -e avahi_needlinuxtool ] && tar -zxvf ../avahi_needlinuxtool.tar.gz
[ ! -e /tmp/avahi_needlinuxtool ] && ln -s `pwd`/avahi_needlinuxtool /tmp/avahi_needlinuxtool
export PATH=`pwd`/avahi_needlinuxtool/bin:$PATH


./configure --host=${CC%-*}  --prefix= --with-distro=none --disable-glib --disable-gtk3 --disable-gobject --disable-qt3 --disable-qt4 --disable-gtk --disable-dbus --disable-python --disable-stack-protector --with-avahi-user=root --with-avahi-group=root --disable-gdbm CPPFLAGS="-I`pwd`/../tmp_install/include" LDFLAGS="-L`pwd`/../tmp_install/lib" LIBDAEMON_CFLAGS='-D_REENTRANT -I$(shell while [ ! -e "configure" ] ; do cd .. ; done ; echo `pwd` )/../tmp_install/include' LIBDAEMON_LIBS='-L$(shell while [ ! -e "configure" ] ; do cd .. ; done ; echo `pwd` )/../tmp_install/lib -ldaemon'
make CFLAGS+="-DNOAVAHILOG"

#make
make install DESTDIR=`pwd`/tmp_install
${CC%-*}-strip tmp_install/lib/libavahi-common.so.3.5.3
${CC%-*}-strip tmp_install/lib/libavahi-core.so.7.0.2
${CC%-*}-strip tmp_install/sbin/avahi-daemon

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
