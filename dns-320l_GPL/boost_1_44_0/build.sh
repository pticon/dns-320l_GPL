#!/bin/sh

export PATH=$PATH:${PWD}/../boost-jam-3.1.18/bin.linuxx86/

BOOST_ROOT=${PWD}
echo "BOOST_ROOT=$BOOST_ROOT"
export BOOST_ROOT=${PWD}

BOOST_BUILD_PATH=${PWD}/tools/build/v2
echo "BOOST_BUILD_PATH=$BOOST_BUILD_PATH"
export BOOST_BUILD_PATH=${PWD}/tools/build/v2

echo "*****BUILDING BOOST 1.44.0*****"

if [ "$1" == "clean" ]; then
	bjam clean
	exit 0
fi

bjam -q --user-config=user-config.jam -sNO_BZIP2=1 -sNO_ZLIB=1 variant=release optimization=space debug-symbols=off --layout=system threading=single runtime-link=shared link=shared --prefix=${PWD}/../xinst --without-python --without-graph --without-wave --without-math --without-mpi --without-signals --without-serialization --without-program_options --without-regex --without-random --withou-graph_parallel --without-iostreams 

bjam -q --user-config=user-config.jam -sNO_BZIP2=1 -sNO_ZLIB=1 variant=release optimization=space debug-symbols=off --layout=system threading=single runtime-link=shared link=shared --prefix=${PWD}/../xinst --without-python --without-graph --without-wave --without-math --without-mpi --without-signals --without-serialization --without-program_options --without-regex --without-random --withou-graph_parallel --without-iostreams install

