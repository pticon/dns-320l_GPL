#!/bin/sh

case "$1" in
	DNS-213|DNS-320|DNS-325)
		./configure --host=arm-linux --without-ncurses --prefix=$PWD
		make clean;make
		;;

	*)
		echo "Usage:$0 { DNS-213|DNS-320|DNS-325 }"
		exit 1
esac
