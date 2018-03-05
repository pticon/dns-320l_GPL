#! /bin/sh

./configure --host=${CC%-*} --enable-maintainer-mode  --disable-acl-support
make clean
make rsync
${CC%-*}-strip -s rsync
