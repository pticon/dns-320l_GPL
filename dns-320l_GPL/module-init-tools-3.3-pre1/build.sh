#!/bin/sh
touch *
./configure --host=arm-linux CC=arm-mv5sft-linux-gnueabi-gcc host_alias=arm-linux --no-create --no-recursion
make clean;make
