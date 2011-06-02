#!/bin/sh
#
################################################################################
##
## Copyright (c) 2011 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
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


if [ "${1}" = "" ] ; then
    echo usage $0 dir-to-store-measuremnts [min-eventsize] [max-eventsize] [event-interval-is-us] [time-for-single-event-size-run-in-seconds]
    exit
fi

make EvalConf MeasurementSubscriber

mkdir -p $1
dir=`pwd`
cd $1

START=$2
if [ "${2}" = "" ] ; then
    START=0
fi

END=$3
if [ "${3}" = "" ] ; then
    END=64
fi

MEAS=$4
if [ "${4}" = "" ] ; then
    MEAS=50
fi

SECO=$5
if [ "${5}" = "" ] ; then
    SECO=20
fi


for i in `seq $START $END`; do
    echo Payload=$i ${SECO}
    ${dir}/EvalConf $i ${MEAS}
    ${dir}/MeasurementSubscriber ${SECO} > EventSize-$i.eval.lat
done

${dir}/preprocess.sh
R --no-save --args ${START} ${END} < ${dir}/analyze.R
${dir}/genplot.sh results.plot > ${dir}/boxes.plot.tex
cd ${dir}
