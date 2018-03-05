#!/bin/sh

unset CFLAGS
unset LDFLAGS
unset LIBS

source ../../xcp.sh


#			-I$(readlink -f ../../glib-2.36.3) \
#			-I$(readlink -f ../../glib-2.36.3/glib)" \
#			-L$(readlink -f ../../glib-2.36.3/glib/.libs) \


xbuild()
{
	make clean ; make distclean

	X_PREFIX=$PWD/../../_xinstall/${PROJECT_NAME}
	XINST_DIR=$(readlink -f $PWD/xinst)

	./configure --prefix=${XINST_DIR} --host=arm-gnu-linux \
		CFLAGS="${CFLAGS} -D_FILE_OFFSET_BITS=64 \
			-I$X_PREFIX/include" \
		LDFLAGS="${LDFLAGS} -s \
			-L$X_PREFIX/lib \
			-L$(readlink -f ../../libiconv-1.9.2/lib/.libs) -liconv \
			-L$(readlink -f ../../libtool-1.5.6/libltdl/.libs)" \
		LIBS="${LIBS} -lglib-2.0 -lltdl" \
		FUSE_CFLAGS="-I$(readlink -f ../../fuse-2.8.4/include)" \
		FUSE_LIBS="-L$(readlink -f ../../fuse-2.8.4/lib/.libs) -lfuse" \
		LIBGPHOTO2_CFLAGS=" \
			-I$(readlink -f ../libgphoto2-2.5.2) \
			-I$(readlink -f ../libgphoto2-2.5.2/gphoto2) \
			-I$(readlink -f ../libgphoto2-2.5.2/libgphoto2_port)" \
		LIBGPHOTO2_LIBS=" \
			-L$(readlink -f ../libgphoto2-2.5.2/libgphoto2/.libs) -lgphoto2 \
			-L$(readlink -f ../libgphoto2-2.5.2/libgphoto2_port/libgphoto2_port/.libs) -lgphoto2_port"
	
	make
	make install
}

# fuse-2.9.3/lib/.libs/libfuse.so.2
# glib-2.36.3/glib/.libs/libglib-2.0.so.0
# CAMLIBS=/lib/camlibs IOLIBS=/lib/iolibs 

xinstall()
{
	${CROSS_COMPILE}strip -s .libs/gphotofs
	xcp .libs/gphotofs ${ROOT_FS}/sbin
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
	echo "Usage : xbuild.sh {build | install | clean}"
fi

