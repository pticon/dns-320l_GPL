#!/bin/bash

unset APPS
unset SCRIPTS
unset LDFLAGS
unset CFLAGS

source ../xcp.sh

xbuild()
{
	#unset CROSS_COMPILE
	#export CFLAGS="-DALPHA_CUSTOMIZE -O2 -pipe"
	#export CC AR RANLIB RC
	
	CC=gcc
	AR=ar
	NM=nm
	RANLIB=ranlib
	
	THIS_ROOT=$(readlink -f $PWD)
	
	
	## Always enable OCF support
	CFLAGS="$CFLAGS -DHAVE_CRYPTODEV -DUSE_CRYPTODEV_DIGESTS"
	
	# We need zlib
	if [ ! -e ../zlib-1.2.3/libz.so.1 ]; then
		cat <<-EOF
	
		ERROR: "../zlib-1.2.3/libz.so.1" does not exist!
		Please build it first.
	
		$ cd ../zlib-1.2.3
		$ ./xbuild build
	
		EOF
	
		exit 1
	fi
	CFLAGS="$CFLAGS -I${THIS_ROOT}/../zlib-1.2.3"
	export LDFLAGS="-L${THIS_ROOT}/../zlib-1.2.3"
	
	
	./Configure linux-generic32 \
		--prefix=/usr \
		--openssldir=/etc/ssl \
		--libdir=/lib \
		threads \
		shared \
		enable-camellia \
		enable-mdc2 \
		enable-tlsext \
		enable-idea \
		no-rc5 \
		zlib \
		--with-zlib-lib="${THIS_ROOT}/../zlib-1.2.3" \
		no-ssl2 \
		no-ssl3
	
	
	CFLAG=$(grep ^CFLAG= Makefile | LC_ALL=C sed \
		-e 's:^CFLAG=::' \
		-e 's:-fomit-frame-pointer ::g' \
		-e 's:-O[0-9] ::g' \
		-e 's:-march=[-a-z0-9]* ::g' \
		-e 's:-mcpu=[-a-z0-9]* ::g' \
		-e 's:-m[a-z0-9]* ::g' \
		)
	
	sed -i \
		-e "/^CFLAG/s|=.*|=${CFLAG} ${CFLAGS}|" \
		-e "/^SHARED_LDFLAGS=/s|$| ${LDFLAGS}|" \
		Makefile
	
	# Dirty workaround, I do not waste my time to find out why.
	sed -i -e 's:.bad:.so:' engines/Makefile
	sed -i -e 's:.bad:.so:' engines/ccgost/Makefile
	
	make -j1 depend
	make all build-shared || exit 1
	
	# rehash is needed to prep the certs/ dir; do this
	# separately to avoid parallel build issues.
	#make rehash
	
	make INSTALL_PREFIX=${THIS_ROOT}/xinst install || exit 1
	make INSTALL_PREFIX=$(readlink -f ${THIS_ROOT}/../_xinstall/${PROJECT_NAME}) install || exit 1
	
	chmod +w xinst/usr/lib/libssl.so.1.0.2
	chmod +w xinst/usr/lib/libcrypto.so.1.0.2
	
	$STRIP -s xinst/usr/lib/libssl.so.1.0.2
	$STRIP -s xinst/usr/lib/libcrypto.so.1.0.2
	$STRIP -s xinst/usr/bin/openssl
	
	#cp -a include ../_xinstall/${PROJECT_NAME}/include/openssl-1.0.1c  # nelson remove it since it generates dead link of headers
	if [ ! -e ../_xinstall/${PROJECT_NAME}/include/openssl ]; then
        mkdir ../_xinstall/${PROJECT_NAME}/include/openssl
	fi
	
	cp -af xinst/usr/lib/* ../_xinstall/${PROJECT_NAME}/lib/
	mkdir -p ../_xinstall/${PROJECT_NAME}/include/openssl
	cp -af xinst/usr/include/openssl/* ../_xinstall/${PROJECT_NAME}/include/openssl/
	
#	cat <<EOF
	
#	Compile successfully!
	
#	Now you should copy what you need to your rootfs.
	
#	e.g.
	
#	  $ cp xinst/usr/lib/libssl.so.1.0.0 \$ROOT_FS/lib/
#	  $ cp xinst/usr/lib/libcrypto.so.1.0.0 \$ROOT_FS/lib/
	
#	  $ cp xinst/usr/lib/libssl.so.1.0.0 \$ROOT_FS/sbin
	
	#EOF
}

xinstall()
{
	xcp xinst/usr/lib/libssl.so.1.0.2 $XLIB_DIR
	xcp xinst/usr/lib/libcrypto.so.1.0.2 $XLIB_DIR
	xcp xinst/usr/lib/libssl.so.1.0.2 $ROOT_FS/lib/
	xcp xinst/usr/lib/libcrypto.so.1.0.2 $ROOT_FS/lib/
	xcp xinst/usr/bin/openssl $ROOT_FS/sbin/
	
	cp -rf xinst/usr/* ../_xinstall/${PROJECT_NAME}/

}

xclean()
{
	git clean -dfx
	git checkout -- .
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
