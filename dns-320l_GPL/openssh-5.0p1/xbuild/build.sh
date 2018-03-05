#!/bin/sh

X_STRIP=${CROSS_COMPILE}strip

make clean
make distclean

../configure --host=arm-linux --prefix=${PWD}/.install LD=$(CC)

make

$X_STRIP scp sftp sftp-server ssh ssh-add ssh-agent sshd ssh-keygen ssh-keyscan ssh-keysign ssh-rand-helper
