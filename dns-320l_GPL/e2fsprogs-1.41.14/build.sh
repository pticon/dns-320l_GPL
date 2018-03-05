make clean
make distclean

./configure --host=arm-linux --disable-debugfs --disable-imager --enable-resizer --enable-fsck --disable-nls --disable-e2initrd-helper --disable-elf-shlibs

make
