make distclean
rm -rf autom4te.cache/
rm -rf tmp_install
rm -rf ../tmp_install
rm -f  po/ca.gmo po/de.gmo  po/id.gmo  po/lt.gmo  po/pt_BR.gmo  po/sv.gmo
git checkout po configure config.h.in~ config.h.in


(cd `pwd`/../libiconv-1.9.2 ; make distclean )
#(cd `pwd`/../libiconv-1.9.2 ; make distclean ; git checkout Makefile)
(cd doc;cp tmpneedfile/* .)
(cd `pwd`/../ADS/ncurses-5.7 ; make distclean )
