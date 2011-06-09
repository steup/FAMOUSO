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

#include <boost/preprocessor/repetition/repeat.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/stringize.hpp>

#ifndef FAMOUSOCONFIG
    #define  FAMOUSOCONFIG 11_can-config-pubsubman.cc
#endif

#define TemplateEffect
#include BOOST_PP_STRINGIZE(FAMOUSOCONFIG)

#include "mw/api/ExtendedEventChannel.h"
#include "mw/attributes/Latency.h"
#include "mw/attributes/AttributeSeq.h"

#include <avr/io.h>

template<>
inline UID getNodeID<void>(){
    return UID();
}

#ifndef COUNT
    #define COUNT 1
#endif


/*#define DECL(z, n, text) \
    famouso::mw::api::ExtendedEventChannel < \
        famouso::config::PEC, \
        famouso::mw::attributes::AttributeSeq< \
            famouso::mw::attributes::Latency<n> \
        >::type, false \
    > pec ## n (BOOST_PP_STRINGIZE(BOOST_PP_CAT(SUBJEC, n)));
*/

#define DECL(z, n, text) \
    famouso::config::PEC pec ## n (BOOST_PP_STRINGIZE(BOOST_PP_CAT(SUBJEC, n)));

#define USAGE(z, n, text) \
    pec ## n.announce(); \
    famouso::mw::Event event ## n(pec ## n.subject()); \
    pec ## n.publish(event ## n);

BOOST_PP_REPEAT(COUNT, DECL, int)

int main(int argc, char** argv) {

    famouso::init<famouso::config>();

    BOOST_PP_REPEAT(COUNT, USAGE, int)
}
