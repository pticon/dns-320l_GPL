#!/bin/sh
make clean ; make distclean
ac_cv_func_setpgrp_void=yes ./configure LDFLAGS=-s --host=arm-linux-gnu
make 

$STRIP dhcp6c

$STRIP dhcp6ctl
