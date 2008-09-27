#!/bin/sh

pushd `dirname $0`

BJAMCONFIG=" gcc --layout=system --with-system link=static --user-config=`pwd`/user-config.jam.${3%/*} install "
PARAMS=" --libdir=$1 --includedir=$2 "
echo Configure-Parameter $PARAMS
echo BJAM-Parameter $BJAMCONFIG

pushd Boost
./configure $PARAMS
pushd tools/jam/src
arch=`./bootstrap/jam0 -d0 -f build.jam --toolset=gcc --show-locate-target`
popd
make clean
echo ./tools/jam/src/$arch/bjam $BJAMCONFIG $PARAMS
./tools/jam/src/$arch/bjam -d2 -t/home/mschulze/Uni/Research/MIKRO/famouso/lib/i386-mingw32/gcc-3.4.5/libboost_system-mt.a $BJAMCONFIG $PARAMS
#make distclean
popd

for file in $1/* ; do
	mv $file ${file%.*}.fam
	mv ${file%.*}.fam ${file%.*}.a
done

popd
