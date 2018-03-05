
TMP_GPL_PATH=`(cd ../../../..;echo ${PWD})`
[ -z ${PROJECT_NAME} ] && { read -p "Please input project name (ex. DNS-345): " TMP_PROJECT_NAME; }

TMP_MODULE_DIR=${MODULE_DIR=${TMP_GPL_PATH}/${TMP_PROJECT_NAME}/module}
[ -e ${TMP_MODULE_DIR} ] || { echo  project name do not exist ; exit 0; }

TMP_XLIB_DIR=${XLIB_DIR=${TMP_MODULE_DIR}/lib/}
TMP_XINC_DIR=${XINC_DIR=${TMP_MODULE_DIR}/include}

#unicenv gpl
#unset CFLAGS
unset LDFLAGS
unset LIBS
#CFLAGS=${CFLAGS}" -I/home/ibuki/cvmod/check_in/unic/module/include -I/home/ibuki/cvmod/check_in/unic/module/include/sqlite-3.6.14.1"

source ../../../xcp.sh
MY_PREFIX=$PWD/../../../_xinstall/${PROJECT_NAME}
xbuild()
{
if [ ! -e "${MY_PREFIX}/lib/libFLAC.so" ]; then
	sh ../../../flac-1.3.0/xbuild.sh build
fi
#sh needlib.sh
#find ./ * | xargs touch -d `date -d 'today' +%y%m%d`

[ -e Makefile ] || ./configure --host=${CC%-*} --prefix=/usr/local/firefly --enable-sqlite3  --enable-iconv --enable-flac \
ac_cv_func_setpgrp_void=yes \
LDFLAGS="-L${TMP_XLIB_DIR} -lmipc -liconv -L${MY_PREFIX}/lib/ -lFLAC  " \
CPPFLAGS="-I${TMP_XINC_DIR} -I${MY_PREFIX}/include "
#--with-id3tag=$FF_PATH

make
make install prefix=`pwd`/tmp_install
${CC%-*}-strip tmp_install/sbin/mt-daapd
${CC%-*}-strip tmp_install/bin/wavstreamer
${CC%-*}-strip tmp_install/lib/mt-daapd/plugins/*.so

source ./colortab.sh

echo -e $fcolor_green$color_bright
echo '		##############################################'
echo '		# 1.Run "sh xbuild.sh clean" to clean all    #'
echo '		#   dependancy files                         #'
echo '		# 2.Run "sh xbuild.sh install" to install    #'
echo '		#    files to nas folder                     #'
echo '		##############################################'
echo -ne $color_default

}

xinstall()
{
	${CC%-*}-strip tmp_install/sbin/mt-daapd
	${CC%-*}-strip tmp_install/lib/mt-daapd/plugins/*.so
	${CC%-*}-strip tmp_install/bin/wavstreamer

	[ -e ${ROOT_FS}/firefly/firefly/sbin ] || { mkdir -p ${ROOT_FS}/firefly/firefly/sbin ;  }
	xcp tmp_install/sbin/mt-daapd ${ROOT_FS}/firefly/firefly/sbin
	[ -e ${ROOT_FS}/firefly/firefly/lib/mt-daapd/plugins ] || { mkdir -p ${ROOT_FS}/firefly/firefly/lib/mt-daapd/plugins ; }
	cp tmp_install/lib/mt-daapd/plugins/*.so ${ROOT_FS}/firefly/firefly/lib/mt-daapd/plugins
	[ -e ${ROOT_FS}/firefly/firefly/bin ] || { mkdir -p ${ROOT_FS}/firefly/firefly/bin ; }
	xcp tmp_install/bin/wavstreamer ${ROOT_FS}/firefly/firefly/bin
	xcp tmp_install/bin/mt-daapd-ssc.sh ${ROOT_FS}/firefly/firefly/bin
	[ -e ${ROOT_FS}/firefly/firefly/share/mt-daapd/ ] || { mkdir -p ${ROOT_FS}/firefly/firefly/share/mt-daapd/ ; }
	[ -e ${ROOT_FS}/firefly/firefly/share/mt-daapd/admin-root ] || \
	cp tmp_install/share/mt-daapd/admin-root ${ROOT_FS}/firefly/firefly/share/mt-daapd/ -a
}

xinstall_tmp()
{
	tmp_dir="/home/tmp/${USER}/${PROJECT_NAME}/gpl/itune"
	[ -d ${tmp_dir} ] || (umask 000;mkdir  -p ${tmp_dir})
	#firefly
	[ -d ${tmp_dir}/firefly ] || mkdir -m 777  ${tmp_dir}/firefly
	cp tmp_install/* ${tmp_dir}/firefly -a

	#libFLAC
	[ -d ${tmp_dir}/needlib ] || mkdir -m 777  ${tmp_dir}/needlib
	xcp ${MY_PREFIX}/lib/libFLAC.so.8 ${tmp_dir}/needlib
	xcp ${MY_PREFIX}/bin/flac ${tmp_dir}/needlib
}

xclean()
{
	sh clean.sh
	rm -f config.h.in~
}

if [ "$1" = "build" ]; then
   xbuild
elif [ "$1" = "install" ]; then
   xinstall
elif [ "$1" = "tmp" ]; then
   xinstall_tmp
elif [ "$1" = "clean" ]; then
   xclean
else
   echo "Usage : xbuild.sh build or xbuild.sh install or xbuild.sh tmp or xbuild.sh clean"
fi
