#!/bin/sh

unset CFLAGS
unset LDFLAGS
unset LIBS

source ../xcp.sh

xbuild()
{
	#modify Makefile
	make clean
	cat Config | sed 's/IP_CONFIG_SETNS:=y//g' > Config.new
	mv Config.new Config

	make
}

xinstall()
{
	${CROSS_COMPILE}strip -s ip/ip
	xcp ip/ip ${ROOT_FS}/sbin
}

xclean()
{
	make clean
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
