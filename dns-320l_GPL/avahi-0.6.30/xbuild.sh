#!/bin/sh

unset CFLAGS
unset LDFLAGS
unset LIBS

source ${ROOT_FS}/files/project_features


MY_POSITION=${PWD}
MY_PREFIX=${PWD}/../_xinstall/${PROJECT_NAME}

xbuild_dbus()
{
	rm -rf ${PWD}/_xinstall
	sh clean.sh

	#build expat
	if [ ! -e $MY_PREFIX/lib/libexpat.so.1 ]; then
		cd ${MY_POSITION}/../expat-2.0.1/
   		./configure --host=arm-gnu-linux --prefix=${MY_PREFIX}
		make
		make install
		cd $MY_POSITION
	fi

	#build libdaemon
	if [ ! -e $MY_PREFIX/lib/libdaemon.so.0 ]; then
		cd ${MY_POSITION}/../libdaemon-0.14/
    	./configure --host=arm-gnu-linux --prefix=${MY_PREFIX} ac_cv_func_setpgrp_void=yes
		make
		make install
		cd $MY_POSITION
	fi

	#build dbus
	if [ ! -e $MY_PREFIX/lib/libdbus-1.so.3 ]; then
		cd ${MY_POSITION}/../dbus-1.4.20/
    	./configure --host=arm-gnu-linux --prefix=${MY_PREFIX} --enable-abstract-sockets \
		--includedir=${MY_PREFIX}/include --libdir=${MY_PREFIX}/lib \
		CPPFLAGS="-I${MY_PREFIX}/include" \
		LDFLAGS="-L${MY_PREFIX}/lib"
    	make all
		make install
		cp dbus/dbus-arch-deps.h ${MY_PREFIX}/include/dbus-1.0/dbus/.
		cd $MY_POSITION
	fi

    #build avahi
	cd ${MY_POSITION}/../avahi-0.6.30/

	find ./ * | xargs touch -d `date -d 'today' +%y%m%d`

	[ ! -e avahi_needlinuxtool ] && tar -zxvf ../avahi_needlinuxtool.tar.gz
	[ ! -e /tmp/avahi_needlinuxtool ] && ln -s `pwd`/avahi_needlinuxtool /tmp/avahi_needlinuxtool
	export PATH=`pwd`/avahi_needlinuxtool/bin:$PATH

	./configure --host=arm-gnu-linux --prefix= --with-distro=none \
	--disable-glib --disable-gtk3 --disable-gobject --disable-qt3 --disable-qt4 --disable-gtk \
	--disable-python --disable-stack-protector --disable-gdbm --disable-mono --disable-monodoc \
    --with-avahi-user=root --with-avahi-group=root \
	PKG_CONFIG_PATH="${MY_PREFIX}/lib/pkgconfig" \
	CPPFLAGS="-I${MY_PREFIX}/include" LDFLAGS="-L${MY_PREFIX}/lib -ldaemon -ldbus-1"

    make
	make install DESTDIR=${MY_PREFIX}

	rm -f /tmp/avahi_needlinuxtool
}

xinstall_dbus()
{
	${STRIP} ${MY_PREFIX}/lib/libavahi-common.so.3
    ${STRIP} ${MY_PREFIX}/lib/libavahi-core.so.7
	${STRIP} ${MY_PREFIX}/lib/libavahi-client.so.3
	${STRIP} ${MY_PREFIX}/bin/avahi-browse
	${STRIP} ${MY_PREFIX}/sbin/avahi-daemon

	cp ${MY_PREFIX}/lib/libavahi-common.so.3 ${ROOT_FS}/lib/libavahi-common.so.3
	cp ${MY_PREFIX}/lib/libavahi-core.so.7 ${ROOT_FS}/lib/libavahi-core.so.7
	cp ${MY_PREFIX}/lib/libavahi-client.so.3 ${ROOT_FS}/lib/libavahi-client.so.3


	cp ${MY_PREFIX}/bin/avahi-browse ${ROOT_FS}/bin/avahi-browse
	cp ${MY_PREFIX}/sbin/avahi-daemon ${ROOT_FS}/bin/avahi-daemon

}

xbuild()
{
	sh needlib.sh

	find ./ * | xargs touch -d `date -d 'today' +%y%m%d`


	[ ! -e avahi_needlinuxtool ] && tar -zxvf ../avahi_needlinuxtool.tar.gz
	[ ! -e /tmp/avahi_needlinuxtool ] && ln -s `pwd`/avahi_needlinuxtool /tmp/avahi_needlinuxtool
	export PATH=`pwd`/avahi_needlinuxtool/bin:$PATH


    ./configure --host=${CC%-*}  --prefix= --with-distro=none --disable-glib --disable-gtk3 --disable-gobject --disable-qt3 --disable-qt4 --disable-gtk --disable-dbus --disable-python --disable-stack-protector --with-avahi-user=root --with-avahi-group=root --disable-gdbm CPPFLAGS="-I`pwd`/../tmp_install/include" LDFLAGS="-L`pwd`/../tmp_install/lib" LIBDAEMON_CFLAGS='-D_REENTRANT -I$(shell while [ ! -e "configure" ] ; do cd .. ; done ; echo `pwd` )/../tmp_install/include' LIBDAEMON_LIBS='-L$(shell while [ ! -e "configure" ] ; do cd .. ; done ; echo `pwd` )/../tmp_install/lib -ldaemon'
	make CFLAGS+="-DNOAVAHILOG"

	#make
	make install DESTDIR=`pwd`/tmp_install

	rm -f /tmp/avahi_needlinuxtool
}

xinstall()
{
	${STRIP} tmp_install/lib/libavahi-common.so.3
	${STRIP} tmp_install/lib/libavahi-core.so.7
	${STRIP} tmp_install/sbin/avahi-daemon

	cp tmp_install/lib/libavahi-common.so.3 ${ROOT_FS}/lib/libavahi-common.so.3
	cp tmp_install/lib/libavahi-core.so.7 ${ROOT_FS}/lib/libavahi-core.so.7
	cp tmp_install/sbin/avahi-daemon ${ROOT_FS}/bin/avahi-daemon
}

xclean()
{
	sh clean.sh
}


if [ "$1" = "build" ]; then
	if [ "$PROJECT_FEATURE_MYPHOTO" = "1" ] || [ "$PROJECT_FEATURE_MYMUSIC" = "1" ]; then
   		xbuild_dbus
   	else
   		xbuild
   	fi
elif [ "$1" = "install" ]; then
	if [ "$PROJECT_FEATURE_MYPHOTO" = "1" ] || [ "$PROJECT_FEATURE_MYMUSIC" = "1" ]; then
   		xinstall_dbus
   	else
   		xinstall
   	fi
elif [ "$1" = "clean" ]; then
   xclean
else
   echo "Usage : xbuild.sh build or xbuild.sh install or xbuild.sh clean"
fi
