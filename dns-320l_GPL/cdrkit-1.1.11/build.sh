
sh needlib.sh

export PATH=$PATH:`pwd`/../cmake-2.8.4-Linux-i386/bin

make
#make CFLAGS+=" -s"
${CC%-*}-strip -s build/genisoimage/genisoimage
