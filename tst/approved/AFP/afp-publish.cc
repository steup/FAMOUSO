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
#include <signal.h>
#include <fcntl.h>


#include "famouso_bindings.h"

#ifdef AFP_CLASSIC_CONFIG
#include "mw/afp/FragConfigC.h"
#else
#include "mw/afp/FragConfig.h"
#endif
#include "mw/afp/Fragmenter.h"

#include "mw/afp/shared/hexdump.h"


#ifndef AFP_CONFIG
// May be defined using a compiler parameter
#define AFP_CONFIG 1
#endif

using namespace famouso::mw;

class RedundancyTag;
typedef afp::frag::RedundancyAttribute<RedundancyTag, 0> PublishFECRedundancy;


#ifdef AFP_CLASSIC_CONFIG
#if (AFP_CONFIG == 1)
typedef afp::FragConfigC <
    afp::UseEventSeq,
    afp::UseFEC<PublishFECRedundancy>
> MyAFPFragConfig;
#elif (AFP_CONFIG == 2)
typedef afp::FragConfigC <
    afp::UseEventSeq,
    afp::UseNoFEC
> MyAFPFragConfig;
#else
typedef afp::FragConfigC <
    afp::UseNoEventSeq,
    afp::UseNoFEC
> MyAFPFragConfig;
#endif
#else
#if (AFP_CONFIG == 1)
struct SpecialPolicies {
    typedef PublishFECRedundancy RedundancyAttribute;
};
typedef afp::FragConfig <
    afp::packet_loss | afp::FEC,
    SpecialPolicies
> MyAFPFragConfig;
#elif (AFP_CONFIG == 2)
typedef afp::FragConfig <
    afp::packet_loss
> MyAFPFragConfig;
#else
typedef afp::FragConfig <
    afp::ideal_network
> MyAFPFragConfig;
#endif
#endif


int done;
char VideoSubject[] = "_famcat_";


void siginthandler(int egal) {
    done = 1;
}


void publish(const famouso::mw::Event & e, famouso::config::PEC & pec) {
    const uint16_t mtu = 16;
    uint8_t buffer [mtu];

    famouso::mw::Event fragment_e(e.subject);
    fragment_e.data = buffer;

    afp::Fragmenter<MyAFPFragConfig> f(e.data, e.length, mtu);

    if (f.error())
        return;

    fprintf(stderr, "Sending event (%u Bytes):\n", e.length);
    afp::shared::hexdump(e.data, e.length);

    while ( (fragment_e.length = f.get_fragment(fragment_e.data)) ) {

        fprintf(stderr, "Sending fragment (%u Bytes):\n", fragment_e.length);
        afp::shared::hexdump(fragment_e.data, fragment_e.length);

        pec.publish(fragment_e);
//        pec.publish(fragment_e);
    }
}

int main(int argc, char **argv) {
    done = 0;
    signal(SIGINT, siginthandler);

    printf("FAMOUSO -- Initialization started.\n");
    famouso::init<famouso::config>();
    printf("FAMOUSO -- Initialization successful.\n");

    famouso::config::PEC pec(VideoSubject);
    pec.announce();

    famouso::mw::Event e(pec.subject());

// while (!done) {

    PublishFECRedundancy::value() = 10;

    //uint8_t data[] = "abcdefghijklmnopqrstuvwxyz0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    uint8_t data[] = "abcdefghijklmnopqrstuvwx";
    e.length = sizeof(data);
    e.data = data;
    publish(e, pec);

    PublishFECRedundancy::value() = 1;
    uint8_t data2[] = "1234";
    e.length = sizeof(data2);
    e.data = data2;
    publish(e, pec);

    PublishFECRedundancy::value() = 5;
    uint8_t data3[] = "Halli hallo... 1 2 3... Test... TEST...";
    e.length = sizeof(data3);
    e.data = data3;
    publish(e, pec);

// }
}

