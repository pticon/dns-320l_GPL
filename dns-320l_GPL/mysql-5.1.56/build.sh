#!/bin/sh

./configure --host=arm-linux --prefix=/usr --with-charset=utf8 --without-debug --without-docs --without-man --without-bench --with-named-curseslibs=${PWD}/../gd_cluster/xinst ac_cv_sys_restartable_syscalls=yes

