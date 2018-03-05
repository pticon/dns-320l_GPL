#! /bin/sh

./configure --host=arm-mv5sft-linux-gnueabi host_alias=arm-mv5sft-linux-gnueabi CC=arm-mv5sft-linux-gnueabi-gcc CXX=arm-mv5sft-linux-gnueabi-g++ --no-create --no-recursion
make
