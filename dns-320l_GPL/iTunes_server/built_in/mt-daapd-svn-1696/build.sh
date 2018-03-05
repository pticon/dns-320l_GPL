
TMP_GPL_PATH=`(cd ../../../..;echo ${PWD})`
[ -z ${PROJECT_NAME} ] && { read -p "Please input project name (ex. DNS-345): " TMP_PROJECT_NAME; }

TMP_MODULE_DIR=${MODULE_DIR=${TMP_GPL_PATH}/${TMP_PROJECT_NAME}/module}
[ -e ${TMP_MODULE_DIR} ] || { echo  project name do not exist ; exit 0; }

TMP_XLIB_DIR=${XLIB_DIR=${TMP_MODULE_DIR}/lib/}
TMP_XINC_DIR=${XINC_DIR=${TMP_MODULE_DIR}/include}

unicenv gpl
unset CFLAGS
unset LDFLAGS
unset LIBS

sh needlib.sh

#find ./ * | xargs touch -d `date -d 'today' +%y%m%d`

./configure --host=${CC%-*} --prefix=/usr/local/firefly --enable-sqlite3  --enable-iconv \
ac_cv_func_setpgrp_void=yes \
LDFLAGS="-L${TMP_XLIB_DIR} -lmipc -liconv" \
CPPFLAGS="-I${TMP_XINC_DIR}"
#--with-id3tag=$FF_PATH

make
make install prefix=`pwd`/tmp_install
${CC%-*}-strip tmp_install/sbin/mt-daapd
${CC%-*}-strip tmp_install/bin/wavstreamer
${CC%-*}-strip tmp_install/lib/mt-daapd/plugins/*.so

source ./colortab.sh

echo -e $fcolor_green$color_bright
echo '		##############################################'
echo '		# 1.Run "sh clean.sh" to clean all           #'
echo '		#   dependancy files                         #'
echo '		# 2.Run "sh .installfile.sh" to install      #'
echo '		#    files to /home/tmp/user                 #'
echo '		# 3.Run "sh .installfile.sh nas" to install  #'
echo '		#    files to nas folder                     #'
echo '		##############################################'
echo -ne $color_default
