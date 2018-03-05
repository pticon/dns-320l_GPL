./configure --host=arm-mv5sft-linux-gnueabi ac_cv_file___dev_urandom_=yes --prefix='$(shell while [ ! -e "configure" ] ; do cd .. ; done ; echo `pwd` )/../tmp_install/' LDFLAGS="-L`pwd`/../tmp_install/lib" CPPFLAGS="-I`pwd`/../tmp_install/include"
make 
make install
arm-mv5sft-linux-gnueabi-strip ../tmp_install/lib/libcurl.so.4.1.1
