
TMP_GPL_PATH=`(cd ../../../..;echo ${PWD})`
[ -z ${PROJECT_NAME} ] && { read -p "Please input project name (ex. DNS-345): " TMP_PROJECT_NAME; }

TMP_MODULE_DIR=${MODULE_DIR=${TMP_GPL_PATH}/${TMP_PROJECT_NAME}/module}
[ -e ${TMP_MODULE_DIR} ] || { echo  project name do not exist ; exit 0; }

TMP_XLIB_DIR=${XLIB_DIR=${TMP_MODULE_DIR}/lib/}
TMP_XINC_DIR=${XINC_DIR=${TMP_MODULE_DIR}/include}

unset CFLAGS
unset LDFLAGS
unset LIBS

./configure --host=arm-mv5sft-linux-gnueabi --prefix=/usr/local/firefly --enable-sqlite3  --enable-iconv \
ac_cv_func_setpgrp_void=yes \
LDFLAGS="-L${TMP_XLIB_DIR} -lmipc -liconv" \
CPPFLAGS="-I${TMP_XINC_DIR}"
#--with-id3tag=$FF_PATH


