#!/bin/sh

unset CFLAGS
unset LDFLAGS
unset LIBS

INST_TMP=$(readlink -f $PWD/../_xinstall/${PROJECT_NAME})

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
	make distclean

	CFLAGS=-I${PWD}/../tcp-wrappers-7.6.dbs/upstream/tarballs/tcp_wrappers_7.6 \
	LDFLAGS=-L${PWD}/../tcp-wrappers-7.6.dbs/upstream/tarballs/tcp_wrappers_7.6 \
	./configure --host=arm-linux --prefix=${PWD}/xinst/ --enable-mount=no \
	--enable-static=no --enable-shared=yes --disable-gss \
	--disable-nfsv4 --disable-uuid

	make
}

xinstall()
{
	make install

	${CROSS_COMPILE}strip -s ${PWD}/xinst/sbin/*

	cp ${PWD}/xinst/sbin/exportfs ${ROOT_FS}/sbin/
	cp ${PWD}/xinst/sbin/rpc.mountd ${ROOT_FS}/sbin/
	cp ${PWD}/xinst/sbin/rpc.nfsd ${ROOT_FS}/sbin/
	cp ${PWD}/xinst/sbin/rpc.statd ${ROOT_FS}/sbin/

	rm -rf ${PWD}/xinst
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
