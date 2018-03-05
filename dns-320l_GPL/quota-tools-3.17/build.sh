#./configure --host=arm-mv5sft-linux-gnueabi LDFLAGS="-L`pwd`/../tmp_install/lib"
./configure --host=arm-mv5sft-linux-gnueabi prefix=`pwd`/tmp_install
make
#make install prefix=`pwd`/tmp_install
make install STRIP=""
chmod 755 tmp_install/sbin/*
arm-mv5sft-linux-gnueabi-strip tmp_install/sbin/*
