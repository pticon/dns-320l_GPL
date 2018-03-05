make clean
#LDFLAGS="${LDFLAGS} -L${XLIB_DIR}"
#CFLAGS="${CFLAGS} -I${XINC_DIR}"
make LDFLAGS="${LDFLAGS} -L${XLIB_DIR}/mysql"
#make -f Makefile_mipsel
