#!/bin/sh
make clean
make clobber
make
arm-mv5sft-linux-gnueabi-strip syslogd
