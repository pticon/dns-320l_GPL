make distclean
rm -rf tmp_install
rm -rf ../tmp_install
rm -f  po/ca.gmo po/de.gmo  po/id.gmo  po/lt.gmo  po/pt_BR.gmo  po/sv.gmo

(cd `pwd`/../libiconv-1.9.2 ; make distclean )
(cd doc;cp tmpneedfile/* .)

