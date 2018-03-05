#!/bin/sh

unset CFLAGS
unset LDFLAGS
unset LIBS

GPL_PREFIX=${PWD}/../_xinstall/${PROJECT_NAME}
mkdir -p ${GPL_PREFIX}

build()
{
	make clean

	CFLAGS="$CFLAGS -I${GPL_PREFIX}/include" \
	LDFLAGS="$LDFLAGS -L${GPL_PREFIX}/lib" \
	make gdisk
	if [ $? != 0 ]; then
		echo ""
		echo -e "***************************"
		echo -e "make failed"
		echo ""
		exit 1
	fi
}

install()
{
	${CROSS_COMPILE}strip -s gdisk
	cp -avf gdisk ${ROOT_FS}/bin
}

clean()
{
	make clean
}


if [ "$1" = "build" ]; then
	build
elif [ "$1" = "install" ]; then
	install
elif [ "$1" = "clean" ]; then
	clean
else
	echo "Usage : $0 build or $0 install or $0 clean"
fi
