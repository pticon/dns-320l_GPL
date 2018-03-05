#!/bin/sh

case "$1" in
	213|320|322|325|345)
		./configure --host=arm-linux
		make clean;make
		;;

	*)
		echo "Usage:$0 { 213|320|322|325|345 }"
		exit 1
esac
