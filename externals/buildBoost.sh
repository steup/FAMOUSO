################################################################################
##
## Copyright (c) 2008, 2009 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
## All rights reserved.
##
##    Redistribution and use in source and binary forms, with or without
##    modification, are permitted provided that the following conditions
##    are met:
##
##    * Redistributions of source code must retain the above copyright
##      notice, this list of conditions and the following disclaimer.
##
##    * Redistributions in binary form must reproduce the above copyright
##      notice, this list of conditions and the following disclaimer in
##      the documentation and/or other materials provided with the
##      distribution.
##
##    * Neither the name of the copyright holders nor the names of
##      contributors may be used to endorse or promote products derived
##      from this software without specific prior written permission.
##
##
##    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
##    IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
##    TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
##    PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
##    OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
##    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
##    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
##    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
##    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
##    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
##    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
##
##
## $Id$
##
################################################################################

#!/bin/sh

pushd `dirname $0`

[ ! -d Boost ] && sh wget.sh
if [ -d Boost ]; then

if [ x${3%.*} = xwindows ]; then
    THREADAPI="threadapi=win32 target-os=windows"
fi

PARAMS=" --libdir=$1 --includedir=$2 "
BJAMCONFIG=" gcc --layout=system --with-system --with-thread --with-program_options link=static --user-config=`pwd`/user-config.jam.${3%/*} install ${THREADAPI}"
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

