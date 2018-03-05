Compiling procedure:
1. build db-4.7.25 module first. (Using build.sh to compile the module)
2. build netatalk-2.0.5 module.
The configure command:
./configure --host=arm-linux --prefix=/ --with-bdb=$PWD/../db-4.7.25/build_unix/install  --enable-cups=no --enable-force-uidgid --enable-afp3 --with-ssl-dir=$PWD/../openssl-0.9.7 --enable-quota=yes --with-shadow --with-cnid-cdb-backend --with-cnid-dbd-backend --enable-krbV-uam

PS: Please build the modules on swtest1.