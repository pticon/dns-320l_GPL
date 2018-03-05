#!/bin/sh

MY_POSITION=${PWD}

#build libpng
cd ${MY_POSITION}/../libpng-1.2.39/
./build.sh
cd $MY_POSITION

#build cryptopp552
cd ${MY_POSITION}/../cryptopp552/
./build.sh
cd $MY_POSITION

#build wxWidgets
cd ${MY_POSITION}/../wxWidgets-2.8.9/
./build.sh
cd $MY_POSITION


PREFIX=$PWD/../

./configure --host=arm-gnu-linux --enable-amulecmd --enable-ed2k --enable-amule-daemon --enable-webserver --disable-amule-gui --disable-monolithic --with-wx-config=`pwd`/../wxWidgets-2.8.9/wx-config --with-libpng-config=`pwd`/../libpng-1.2.35/libpng-config --with-crypto-prefix="`pwd`/../cryptopp552" CPPFLAGS="-I${PWD}/../xinst/include -I$PREFIX/include" LDFLAGS="-L${PWD}/../xinst/lib"

make clean;make CRYPTOPP_LIBS=$PREFIX/cryptopp552/libcryptopp.a CPPFLAGS="-I$PREFIX/xinst/include -I$PREFIX/include"  LDFLAGS="-L$PREFIX/xinst/lib -L$PREFIX/lib -lpng12"

