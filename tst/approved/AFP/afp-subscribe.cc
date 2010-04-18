/*******************************************************************************
 *
 * Copyright (c) 2009-2010 Philipp Werner <philipp.werner@st.ovgu.de>
 *                         Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
 * All rights reserved.
 *
 *    Redistribution and use in source and binary forms, with or without
 *    modification, are permitted provided that the following conditions
 *    are met:
 *
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in
 *      the documentation and/or other materials provided with the
 *      distribution.
 *
 *    * Neither the name of the copyright holders nor the names of
 *      contributors may be used to endorse or promote products derived
 *      from this software without specific prior written permission.
 *
 *
 *    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 *    IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 *    TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 *    PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *    OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *
 * $Id$
 *
 ******************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "famouso_bindings.h"
#include "util/Idler.h"

#ifdef AFP_CLASSIC_CONFIG
#include "mw/afp/DefragConfigC.h"
#else
#include "mw/afp/DefragConfig.h"
#endif
#include "mw/afp/Defragmentation.h"
#include "mw/afp/shared/hexdump.h"


#ifndef AFP_CONFIG
// May be defined using a compiler parameter
#define AFP_CONFIG 1
#endif

using namespace famouso::mw;

#ifdef AFP_CLASSIC_CONFIG
#if (AFP_CONFIG == 1)
typedef afp::DefragConfigC <
    afp::OneSubject,
    afp::EventSeqSupport,
    afp::DuplicateChecking,
    afp::ReorderingSupport,
    afp::FECSupport
> MyAFPDefragConfig;
#elif (AFP_CONFIG == 2)
typedef afp::DefragConfigC <
    afp::OneSubject,
    afp::EventSeqSupport,
    afp::NoDuplicateChecking,
    afp::ReorderingSupport,
    afp::NoFECSupport
> MyAFPDefragConfig;
#else
typedef afp::DefragConfigC <
    afp::OneSubject,
    afp::NoEventSeqSupport,
    afp::NoDuplicateChecking,
    afp::NoReorderingSupport,
    afp::NoFECSupport
> MyAFPDefragConfig;
#endif
#else
#if (AFP_CONFIG == 1)
typedef afp::DefragConfig <
    afp::one_subject |
    afp::packet_loss | afp::reordering | afp::duplicates |
    afp::FEC
> MyAFPDefragConfig;
#elif (AFP_CONFIG == 2)
typedef afp::DefragConfig <
    afp::one_subject |
    afp::packet_loss | afp::reordering
> MyAFPDefragConfig;
#else
typedef afp::DefragConfig <
    afp::one_subject |
    afp::ideal_network
> MyAFPDefragConfig;
#endif
#endif


typedef afp::DefragmentationProcessor<MyAFPDefragConfig> DefragmentationProcessor;
typedef afp::DefragmentationStep<MyAFPDefragConfig> DefragmentationStep;


DefragmentationProcessor dp(16);

char VideoSubject[] = "_famcat_";



void VideoCB(famouso::mw::api::SECCallBackData& cbd) {

    fprintf(stderr, "Incoming fragment (%u Bytes):\n", cbd.length);
    afp::shared::hexdump(cbd.data, cbd.length);

    DefragmentationStep ds(cbd.data, cbd.length);

    dp.process_fragment(ds);

    if (ds.event_complete()) {
        famouso::mw::Event e(cbd.subject);
        e.data = ds.get_event_data();
        e.length = ds.get_event_length();

        fprintf(stderr, "Incoming event (%u Bytes):\n", e.length);
        afp::shared::hexdump(e.data, e.length);

        dp.event_processed(ds);
    }
}


int main(int argc, char **argv) {

    printf("FAMOUSO -- Initialization started.\n");
    famouso::init<famouso::config>();
    printf("FAMOUSO -- Initialization successful.\n");

    famouso::config::SEC sec(VideoSubject);
    sec.callback.bind<VideoCB>();
    sec.subscribe();

    Idler::idle();
}
