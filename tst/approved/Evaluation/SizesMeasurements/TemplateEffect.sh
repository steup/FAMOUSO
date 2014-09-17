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
    echo How many different measurements do you need?
    echo usage: $0 count
    exit
fi

rm -f Publisher.table  Subscriber.table

echo -e "inst\ttext\tdata\tbss\ttotal\thex\tname" >> Publisher.table
echo -e "inst\ttext\tdata\tbss\ttotal\thex\tname" >> Subscriber.table
for i in `seq 1 $1` ; do
    make ADD_FLAGS=-DCOUNT=$i -B TemplateEffectPublisher
    mv TemplateEffectPublisher TemplateInstantiationsPublisher.$i
    echo -e $i `size TemplateInstantiationsPublisher.$i | tail -1` | \
        awk -F " " '{print $1"\t"$2"\t"$3"\t"$4"\t"$5"\t"$6"\t"$7}' >>Publisher.table

    make ADD_FLAGS=-DCOUNT=$i -B TemplateEffectSubscriber
    mv TemplateEffectSubscriber TemplateInstantiationsSubscriber.$i
    echo -e $i `size TemplateInstantiationsSubscriber.$i | tail -1` | \
        awk -F " " '{print $1"\t"$2"\t"$3"\t"$4"\t"$5"\t"$6"\t"$7}' >>Subscriber.table
done

cat  Publisher.table  Subscriber.table