

 [ -z ${PROJECT_NAME} ] && { echo  "please \"export PROJECT_NAME\" first"; exit 0; } || \
												{ echo  "PROJECT_NAME = ${PROJECT_NAME}" ;}



TMP_PROJECT_ROOT=${PROJECT_ROOT=`(cd ../..;pwd)`}

TMP_MODULE_DIR=${MODULE_DIR=${TMP_PROJECT_ROOT}/${PROJECT_NAME}/module}
[ -e ${TMP_MODULE_DIR} ] || { echo  project name do not exist ; exit 0; }

TMP_XLIB_DIR=${XLIB_DIR=${TMP_MODULE_DIR}/lib/}

TMP_ROOT_FS=${ROOT_FS=${TMP_MODULE_DIR}/crfs/crfs}

#tmp_dir="/home/tmp/${USER}/${PROJECT_NAME}/gpl/`basename ${PWD}`"
tmp_dir="/home/tmp/${USER}/${PROJECT_NAME}/gpl/itune"

#run cmd "sh installfile.sh nas" to cp file to nas
source ./colortab.sh
echo -e $fcolor_green$color_bright

if [ "${1}" = "nas" ]; then
	cp tmp_install/sbin/mt-daapd ${TMP_ROOT_FS}/firefly/firefly/sbin -v
	cp tmp_install/lib/mt-daapd/plugins/*.so ${TMP_ROOT_FS}/firefly/firefly/lib/mt-daapd/plugins -v
	cp tmp_install/bin/wavstreamer ${TMP_ROOT_FS}/firefly/firefly/bin -v
fi

#cp file to tmp
mkdir -m 777 -p ${tmp_dir}
cp tmp_install/sbin/mt-daapd ${tmp_dir}

echo -ne $color_default

