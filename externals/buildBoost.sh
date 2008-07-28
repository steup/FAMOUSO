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
echo ./tools/jam/src/$arch/bjam $BJAMCONFIG $PARAMS
./tools/jam/src/$arch/bjam $BJAMCONFIG $PARAMS
#make distclean
popd

for file in $1/* ; do
	mv $file ${file%.*}.fam
	mv ${file%.*}.fam ${file%.*}.a
done

popd
