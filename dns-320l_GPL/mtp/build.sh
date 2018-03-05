#!/bin/sh
echo make  libltdl, libusb first!

cd popt-1.16
./build.sh
cd ..

cd libexif-0.6.20
./build.sh
cd ..

cd libgphoto2-2.4.10.1
./build.sh
cd ..

cd gphoto2-2.4.10
./build.sh
cd ..