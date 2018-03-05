#!/bin/bash

export PATH=$PATH:/opt_gccarm/SDK5/armv7-marvell-linux-gnueabi-softfp/bin
export ARCH=arm
export CROSS_COMPILE=arm-marvell-linux-gnueabi-
export CROSS_COMPILE_BH=arm-marvell-linux-gnueabi-
export ALPHA_UBOOT_VERSION=1.5

./build.pl -f nand -v 2013_Q2.0 -b armada_370_rd -i nand -c -o $(pwd)
