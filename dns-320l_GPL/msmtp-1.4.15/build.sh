./needlib.sh

./configure --host=${CC%-*} --with-libssl-prefix=`pwd`/../tmp_install/
make
make install DESTDIR=`pwd`/tmp_install
${CC%-*}-strip tmp_install/usr/local/bin/msmtp
