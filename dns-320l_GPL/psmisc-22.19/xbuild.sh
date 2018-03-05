#!/bin/sh

source ../xcp.sh

xbuild()
{
   CURSES_XINST=$(readlink -f $PWD/../gd_cluster/xinst)

   export CFLAGS="-I$CURSES_XINST/include -I$CURSES_XINST/include/ncurses"
   export LDFLAGS="-L$CURSES_XINST/lib"
   
   ./configure --host=${TARGET_HOST} --disable-nls \
   	ac_cv_func_malloc_0_nonnull=yes \
   	ac_cv_func_realloc_0_nonnull=yes
   
   make
#   echo ""; echo ""; echo "";
#   echo "Now type make!"
#   echo "";
#   echo "If successfully built, do:"
#   echo "\${STRIP} -s src/fuser"
#   echo "";

}

xinstall()
{
   ${CROSS_COMPILE}strip -s src/fuser
   xcp src/fuser ${ROOT_FS}/bin
}

if [ "$1" = "build" ]; then
   xbuild
elif [ "$1" = "install" ]; then
   xinstall
else
   echo "Usage : xbuild.sh build or xbuild.sh install"
fi
