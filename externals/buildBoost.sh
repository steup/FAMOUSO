#!/bin/sh

pushd `dirname $0`

[ ! -d Boost ] && sh wget.sh
if [ -d Boost ]; then

if [ x${3%.*} = xwindows ]; then
    THREADAPI="threadapi=win32"
fi

PARAMS=" --libdir=$1 --includedir=$2 "
BJAMCONFIG=" gcc --layout=system --with-system --with-thread link=static --user-config=`pwd`/user-config.jam.${3%/*} install ${THREADAPI}"
echo Configure-Parameter $PARAMS
echo BJAM-Parameter $BJAMCONFIG

pushd Boost
[ ! -x tools/jam/src/bootstrap/jam0 ] && ./configure $PARAMS
pushd tools/jam/src
arch=`./bootstrap/jam0 -d0 -f build.jam --toolset=gcc --show-locate-target`
popd
command="./tools/jam/src/$arch/bjam -d0 -j10 $BJAMCONFIG $PARAMS"
make clean
rm -f user-config.*
echo $command
$command
#make distclean
popd

for file in $1/* ; do
	mv $file ${file%.*}.fam
	mv ${file%.*}.fam ${file%.*}.a
done


fi

popd

