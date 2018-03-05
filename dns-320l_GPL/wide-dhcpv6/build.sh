#!/bin/sh
make clean ; make distclean
#if [ "$PROJECT_NAME" == "DNS-327L" ]; then
	ac_cv_func_setpgrp_void=yes LEXLIB=../flex-2.5.36/libfl.a ./configure LDFLAGS=-s --host=arm-linux-gnu
#else
#	ac_cv_func_setpgrp_void=yes ./configure LDFLAGS=-s --host=arm-linux-gnu
#fi

make 

$STRIP dhcp6c

$STRIP dhcp6ctl
