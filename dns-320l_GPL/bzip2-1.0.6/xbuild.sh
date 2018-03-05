
unset CFLAGS
unset LDFLAGS
unset LIBS


xbuild()
{
	make clean
	make
	make PREFIX=$(readlink -f $PWD/../_xinstall/${PROJECT_NAME}) install
}

xinstall()
{
	return
}

xclean()
{
	make clean
}

if [ "$1" = "build" ]; then
   xbuild
elif [ "$1" = "install" ]; then
   xinstall
elif [ "$1" = "clean" ]; then
   xclean
else
   echo "Usage : xbuild.sh build or xbuild.sh install or xbuild.sh clean"
fi
