#!/bin/sh
#!/bin/sh

xbuild()
{
	echo -e "\e[31mproject name = ${PROJECT_NAME}\e[0m"
	echo -e "\e[31mpwd = ${PWD}\e[0m"

    cmake -DCMAKE_INSTALL_PREFIX=$PWD/../_xinstall/${PROJECT_NAME} \
	-DYAJL_INCLUDE_DIR=$PWD/../_xinstall/${PROJECT_NAME}/include/ \
	-DYAJL_LIBRARY=$PWD/../_xinstall/${PROJECT_NAME}/lib/libyajl.so \
    	-DLIBGCRYPTCONFIG_EXECUTABLE=$PWD/../_xinstall/${PROJECT_NAME}/bin/libgcrypt-config \
    	-DIBERTY_LIBRARY=$PWD/../_xinstall/${PROJECT_NAME}/lib/libiberty.a \
    	-DJSONC_INCLUDE_DIR=$PWD/../_xinstall/${PROJECT_NAME}/include/ \
    	-DJSONC_LIBRARY=$PWD/../_xinstall/${PROJECT_NAME}/lib/libjson-c.so.2 \
    	-DZLIB_INCLUDE_DIR=$PWD/../_xinstall/${PROJECT_NAME}/include/ \
    	-DZLIB_LIBRARY=$PWD/../_xinstall/${PROJECT_NAME}/lib/libz.so \
    	-DBoost_INCLUDE_DIR=$PWD/../_xinstall/${PROJECT_NAME}/include/ \
    	-DBoost_LIBRARY_DIRS=$PWD/../_xinstall/${PROJECT_NAME}/lib/ \
    	-DBoost_FILESYSTEM_LIBRARY=$PWD/../_xinstall/${PROJECT_NAME}/lib/libboost_filesystem-mt.so \
    	-DBoost_FILESYSTEM_LIBRARY_DEBUG=$PWD/../_xinstall/${PROJECT_NAME}/lib/libboost_filesystem-mt.so \
    	-DBoost_FILESYSTEM_LIBRARY_RELEASE=$PWD/../_xinstall/${PROJECT_NAME}/lib/libboost_filesystem-mt.so \
    	-DBoost_PROGRAM_OPTIONS_LIBRARY=$PWD/../_xinstall/${PROJECT_NAME}/lib/libboost_program_options-mt.so \
    	-DBoost_PROGRAM_OPTIONS_LIBRARY_DEBUG=$PWD/../_xinstall/${PROJECT_NAME}/lib/libboost_program_options-mt.so \
    	-DBoost_PROGRAM_OPTIONS_LIBRARY_RELEASE=$PWD/../_xinstall/${PROJECT_NAME}/lib/libboost_program_options-mt.so \
    	-DBoost_SYSTEM_LIBRARY=$PWD/../_xinstall/${PROJECT_NAME}/lib/libboost_system-mt.so \
    	-DBoost_SYSTEM_LIBRARY_DEBUG=$PWD/../_xinstall/${PROJECT_NAME}/lib/libboost_system-mt.so \
    	-DBoost_SYSTEM_LIBRARY_RELEASE=$PWD/../_xinstall/${PROJECT_NAME}/lib/libboost_system-mt.so .
    
    
    make clean
    make
    #make install
    
    
    if [ $? = 0 ]; then
        echo -e "\e[31mbuild success!\e[0m"
    else
        echo -e "\e[31mbuild fail!\e[0m"
    fi
}

xinstall()
{
    echo -e "\e[35mInstall to rootfs\e[0m"
    ${CROSS_COMPILE}strip -v -s ./grive/grive2 -o ${ROOT_FS}/bin/grive
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


