make distclean
rm -rf tmp_install
rm -rf ../tmp_install
rm -rf autom4te.cache
git checkout config.h.in
git checkout configure

#(cd `pwd`/../libiconv-1.9.2 ; make distclean )
#(cd doc;cp tmpneedfile/* .)
#(cd `pwd`/../ncurses-5.5 ; make distclean )
