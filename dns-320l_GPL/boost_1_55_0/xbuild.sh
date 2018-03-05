#!/bin/sh

BOOST_ROOT=${PWD}
echo "BOOST_ROOT=$BOOST_ROOT"
export BOOST_ROOT=${PWD}

BOOST_BUILD_PATH=${PWD}/tools/build/v2
echo "BOOST_BUILD_PATH=$BOOST_BUILD_PATH"
export BOOST_BUILD_PATH=${PWD}/tools/build/v2

echo "*****BUILDING BOOST 1.55.0*****"

xbuild()
{
    if [ ! -e bjam ]; then
        echo -e "\e[31mGenerate bjam\e[0m"
        ./bootstrap.sh                
    fi  
    if [ ! -e user-config.jam ]; then
        echo -e "\e[31mGenerate user-config.jam \e[0m"
        echo "using gcc : 4.6.4 : ${CROSS_COMPILE}g++ ;" >> user-config.jam
     fi  
    echo -e "\e[31mBuilding boost\e[0m"
    ./bjam -q --user-config=user-config.jam \
    -sNO_BZIP2=1 \
    -sNO_ZLIB=1 \
    variant=release \
    optimization=space \
    debug-symbols=off \
    --layout=tagged \
    threading=multi \
    runtime-link=shared \
    link=shared \
    --prefix=${PWD}/../_xinstall/${PROJECT_NAME} \
    --without-python \
    --without-graph \
    --without-wave \
    --without-math \
    --without-mpi \
    --without-signals \
    --without-serialization \
    --without-regex \
    --without-random \
    --withou-graph_parallel \
    --without-iostreams 
    #--with-program_options \
    
    ./bjam -q --user-config=user-config.jam \
    -sNO_BZIP2=1 \
    -sNO_ZLIB=1 \
    variant=release \
    optimization=space \
    debug-symbols=off \
    --layout=tagged \
    threading=multi \
    runtime-link=shared \
    link=shared \
    --prefix=${PWD}/../_xinstall/${PROJECT_NAME} \
    --without-python \
    --without-graph \
    --without-wave \
    --without-math \
    --without-mpi \
    --without-signals \
    --without-serialization \
    --without-regex \
    --without-random \
    --withou-graph_parallel \
    --without-iostreams \
    install
    #--with-program_options \
}

xinstall()
{
    echo "install"
}

xclean()
{
   	./bjam clean
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


