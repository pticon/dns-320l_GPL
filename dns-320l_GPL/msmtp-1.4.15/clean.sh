make distclean
rm -rf autom4te.cache/
git checkout config.h.in config.h.in~ configure
rm -rf tmp_install
rm -rf ../tmp_install
(cd `pwd`/../openssl-1.0.1c ; make clean )
(cd `pwd`/../zlib-1.2.3 ; make clean ; make distclean )
