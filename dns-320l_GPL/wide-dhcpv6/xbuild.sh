#!/bin/sh

#unset CFLAGS
unset LDFLAGS
unset LIBS

source ../xcp.sh

xbuild()
{
	if [ ! -e ../flex-2.5.36/libfl.a ]; then
		echo make libfl.a first!!!
		#return
		pushd .
		cd ../flex-2.5.36
		./xbuild.sh build
		popd
	fi

	make clean ; make distclean
	ac_cv_func_setpgrp_void=yes LEXLIB=../flex-2.5.36/libfl.a ./configure LDFLAGS=-s --host=${TARGET_HOST}
	make
}

xinstall()
{
	${CROSS_COMPILE}strip -s dhcp6c
	xcp dhcp6c ${ROOT_FS}/bin

	${CROSS_COMPILE}strip -s dhcp6ctl
	xcp dhcp6ctl ${ROOT_FS}/bin
}

xclean()
{
	make clean
	make distclean

	echo clean libfl.a !!!
	pushd .
	cd ../flex-2.5.36
	./xbuild.sh clean
	popd
}

if [ "$1" = "build" ]; then
	xbuild
elif [ "$1" = "install" ]; then
	xinstall
elif [ "$1" = "clean" ]; then
	xclean
else
	echo "Usage : xbuild.sh {build | install | clean}"
fi
