#!/bin/sh

# 
# $ diff unix/Makefile.orig unix/Makefile
#
#  # UnZip flags
# -CC = cc#	try using "gcc" target rather than changing this (CC and LD
# +#CC = cc#	try using "gcc" target rather than changing this (CC and LD
#  LD = $(CC)#	must match, else "unresolved symbol:  ___main" is possible)
# -AS = as
# +AS = $(CC)
#  LOC = $(D_USE_BZ2) $(LOCAL_UNZIP)
#
#
#  linux_noasm:	unix_make
# -       $(MAKE) unzips CC=gcc LD=gcc CFLAGS="-O -Wall"
# +       $(MAKE) unzips CC=$(CC) LD=$(CC) CFLAGS="-O2 -Wall $(CPPFLAGS)"
#

make -f unix/Makefile linux_noasm CPPFLAGS="-DNO_LCHMOD -DUNICODE_SUPPORT -DUNICODE_WCHAR -DUTF8_MAYBE_NATIVE -DLARGE_FILE_SUPPORT"
