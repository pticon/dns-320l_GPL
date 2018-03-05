make clean
rm -rf tmp_install
rm -rf ../tmp_install
(cd `pwd`/../zlib-1.2.3 ; sh xbuild.sh clean )
(cd `pwd`/../openssl-1.0.1c ; sh xbuild.sh clean )
