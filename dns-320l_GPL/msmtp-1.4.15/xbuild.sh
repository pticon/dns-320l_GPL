
unset CFLAGS
unset LDFLAGS
unset LIBS

source ../xcp.sh

xbuild()
{
	./needlib.sh

#	./configure --host=${CC%-*} --with-libssl-prefix=`pwd`/../openssl-1.0.1c/xinst/usr LDFLAGS="-L$PWD/../zlib-1.2.3 -lz"
	./configure --host=${CC%-*} --with-libssl-prefix=`pwd`/../openssl-1.0.1c/xinst/usr LDFLAGS="-L$PWD/../zlib-1.2.3 -lz"

	make
	make install DESTDIR=`pwd`/tmp_install
	${CC%-*}-strip tmp_install/usr/local/bin/msmtp
}

xinstall()
{
	${CC%-*}-strip tmp_install/usr/local/bin/msmtp

	xcp tmp_install/usr/local/bin/msmtp ${ROOT_FS}/bin
}

xclean()
{
	sh clean.sh
}

if [ "$1" = "build" ]; then
   xbuild
elif [ "$1" = "install" ]; then
   xinstall
elif [ "$1" = "clean" ]; then
   xclean
else
   echo "Usage : xbuild.sh build or xbuild.sh install or xbuild.sh clean"
fi