/*******************************************************************************
 *
 * Copyright (c) 2011 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
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
 * $Id: $
 *
 ******************************************************************************/

#include "famouso.h"

#include "mw/nl/voidNL.h"

#include "mw/afp/Config.h"
#include "mw/anl/AbstractNetworkLayer.h"


#include "mw/api/PublisherEventChannel.h"
#include "mw/api/SubscriberEventChannel.h"

#include "mw/el/ml/ManagementLayer.h"
#include "mw/el/EventLayer.h"

namespace famouso {
    class config {
            typedef famouso::mw::nl::voidNL nl;
            typedef famouso::mw::anl::AbstractNetworkLayer<
                        nl,
                        famouso::mw::afp::Disable,
                        famouso::mw::afp::Disable
                    > anl;
        public:
            typedef famouso::mw::el::EventLayer< anl > EL;
            typedef famouso::mw::api::PublisherEventChannel<EL> PEC;
            typedef famouso::mw::api::SubscriberEventChannel<EL> SEC;
    };

}

#include <avr/io.h>

template<>
inline UID getNodeID<void>(){
    return UID();
}

#ifndef COUNT
    #define COUNT 1
#endif

#include <boost/preprocessor/repetition/repeat.hpp>

#define DECL(z, n, text) \
    void cb ## n (famouso::mw::api::SECCallBackData& cbd) {} \
    famouso::config::SEC sec ## n("SUBJECT_");

#define USAGE(z, n, text) \
    sec ## n.subscribe(); \
    sec ## n.callback.bind< cb ##n >();

BOOST_PP_REPEAT(COUNT, DECL, int)

int main(int argc, char** argv) {

    famouso::init<famouso::config>();

    BOOST_PP_REPEAT(COUNT, USAGE, int)
}
