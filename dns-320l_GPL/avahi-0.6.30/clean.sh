make distclean
rm -rf tmp_install
rm -rf avahi_needlinuxtool
rm -f /tmp/avahi_needlinuxtool
rm -rf ../tmp_install
(cd `pwd`/../expat-2.0.1 ; sh clean.sh )
(cd `pwd`/../libdaemon-0.14 ; sh clean.sh )
