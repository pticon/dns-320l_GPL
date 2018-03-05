
source ../xcp.sh

GPL_PREFIX=${PWD}/../_xinstall/${PROJECT_NAME}
mkdir -p ${GPL_PREFIX}

xbuild()
{
	find ./ * | xargs touch -d `date -d 'today' +%y%m%d`
	if [ ! -e configure ] || [ ! -e install-sh ]; then
		./autogen.sh
	fi
	export ac_cv_func_malloc_0_nonnull=yes
	export ac_cv_func_realloc_0_nonnull=yes
	./configure --with-pic --host=${TARGET_HOST} --prefix=${GPL_PREFIX}
	make
	make install
	${STRIP} ${GPL_PREFIX}/lib/libjson.so.0.1.0
	${STRIP} ${GPL_PREFIX}/lib/libjson-c.so.2.0.1
}

xinstall()
{
	[ -e ${GPL_PREFIX}/include/json-c ] || { echo Please run \"sh xbuild.sh build\" first; exit 1;}

	# For WD, libsjon-c is only for Dropbox addon
	if [ "$(has_feature CUSTOM_WD)" = "Yes" ]; then
        xcp ${GPL_PREFIX}/lib/libjson.so.0 ${MODULE_DIR}/apkg/addons/${PROJECT_NAME}/Dropbox/lib
		xcp ${GPL_PREFIX}/lib/libjson.so ${XLIB_DIR}
		xcp ${GPL_PREFIX}/lib/libjson-c.so.2 ${MODULE_DIR}/apkg/addons/${PROJECT_NAME}/Dropbox/lib
		xcp ${GPL_PREFIX}/lib/libjson-c.so ${XLIB_DIR}
	else
		xcp ${GPL_PREFIX}/lib/libjson.so.0 ${ROOT_FS}/lib
		xcp ${GPL_PREFIX}/lib/libjson.so ${XLIB_DIR}
		xcp ${GPL_PREFIX}/lib/libjson-c.so.2 ${ROOT_FS}/lib
		xcp ${GPL_PREFIX}/lib/libjson-c.so ${XLIB_DIR}
	fi
	
	if [ -e ${XINC_DIR}/json ]; then
		rm -rf ${XINC_DIR}/json
	fi
	cp -drf ${GPL_PREFIX}/include/* ${XINC_DIR}
}

xclean()
{
	make distclean
}


COMMAND=$1

case $COMMAND in

	build)
		xbuild
	;;

	install)
		xinstall
	;;

	installtmp)
		xinstall tmp
	;;

	clean)
		xclean
	;;

    *)
		echo "Usage : xbuild.sh build or xbuild.sh install or xbuild.sh clean"
	;;

esac


