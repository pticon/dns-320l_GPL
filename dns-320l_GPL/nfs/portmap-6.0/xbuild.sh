#!/bin/sh

unset CFLAGS
unset LDFLAGS
unset LIBS

INST_TMP=$(readlink -f $PWD/../_xinstall/${PROJECT_NAME})

export CFLAGS=-I${PWD}/../tcp-wrappers-7.6.dbs/upstream/tarballs/tcp_wrappers_7.6 
export LDFLAGS=-L${PWD}/../tcp-wrappers-7.6.dbs/upstream/tarballs/tcp_wrappers_7.6
xbuild()
{
	if [ ! -e ../tcp-wrappers-7.6.dbs/upstream/tarballs/tcp_wrappers_7.6/libwrap.a ]; then
		cat <<-EOF
		ERROR: "../tcp-wrappers-7.6.dbs/upstream/tarballs/tcp_wrappers_7.6/libwrap.a" does not exist!
		Please build it first.
	
		$ cd ../tcp-wrappers-7.6.dbs/upstream/tarballs/tcp_wrappers_7.6
		$ ./xbuild build
	
		EOF
	
		exit 1
	fi

	make clean

	make
}

xinstall()
{
	${CROSS_COMPILE}strip -s portmap
	cp portmap ${ROOT_FS}/sbin/
}

xclean()
{
	make clean
	make distclean
}

if [ "$1" = "build" ]; then
	xbuild
elif [ "$1" = "install" ]; then
	xinstall
elif [ "$1" = "clean" ]; then
	xclean
else
	echo "Usage: xbuild.sh {build|install|clean}"
fi
