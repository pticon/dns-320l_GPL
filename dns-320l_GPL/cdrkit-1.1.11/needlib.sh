sh clean.sh
(cd ../libcap-2.20/ ;sh xbuild.sh build)
(cd ../bzip2-1.0.6/ ;sh xbuild.sh build)

mkdir -p `pwd`/build/include
mkdir -p `pwd`/build/libusal

for var in `ls -d ../zlib-*`
do
echo ${var}

(cd ${var}/ ;sh xbuild.sh build)
cp ${var}/zconf.h ${var}/zlib.h build/include
cp ${var}/libz.so build/libusal
done

#cp ../zlib-1.2.3/zconf.h ../zlib-1.2.3/zlib.h build/include
cp ../bzip2-1.0.6/bzlib.h  build/include
cp ../bzip2-1.0.6/libbz2.a  build/libusal
ln -s `pwd`/../libcap-2.20/libcap/include/sys build/include/
cp ../libcap-2.20/libcap/libcap.a build/libusal