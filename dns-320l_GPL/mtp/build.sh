#!/bin/sh
echo make  libltdl, libusb, jpeg-7,  libpopt, libexif first!

cd libgphoto2-2.4.10.1
./build.sh
cd ..

cd gphoto2-2.4.10
./build.sh
cd ..