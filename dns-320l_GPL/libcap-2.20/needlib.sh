TMP_LIBCAP_DIR=`pwd`
(cd `pwd`/../attr-2.4.46 ; sh build.sh;cp libattr/.libs/libattr.so ${TMP_LIBCAP_DIR}/libcap/)
