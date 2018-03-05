

 [ -z ${PROJECT_NAME} ] && { echo  "please \"export PROJECT_NAME\" first"; exit 0; } || \
												{ echo  "PROJECT_NAME = ${PROJECT_NAME}" ;}



TMP_PROJECT_ROOT=${PROJECT_ROOT=`(cd ../..;pwd)`}

TMP_MODULE_DIR=${MODULE_DIR=${TMP_PROJECT_ROOT}/${PROJECT_NAME}/module}
[ -e ${TMP_MODULE_DIR} ] || { echo  project name do not exist ; exit 0; }

TMP_XLIB_DIR=${XLIB_DIR=${TMP_MODULE_DIR}/lib/}

TMP_ROOT_FS=${ROOT_FS=${TMP_MODULE_DIR}/crfs/crfs}

#tmp_dir="/home/tmp/${USER}/${PROJECT_NAME}/gpl/`basename ${PWD}`"
tmp_dir="/home/tmp/${USER}/${PROJECT_NAME}/rsync3"

#run cmd "sh installfile.sh nas" to cp file to nas
if [ "${1}" = "nas" ]; then
	cp tmp_install/sbin/sshd ${TMP_ROOT_FS}/sbin
	cp tmp_install/bin/ssh ${TMP_ROOT_FS}/sbin
	cp tmp_install/bin/ssh-keygen ${TMP_ROOT_FS}/sbin
fi

#cp file to tmp
mkdir -p ${tmp_dir}
cp tmp_install/sbin/sshd ${tmp_dir}
cp tmp_install/bin/ssh ${tmp_dir}
cp tmp_install/bin/ssh-keygen ${tmp_dir}


