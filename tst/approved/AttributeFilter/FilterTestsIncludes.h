/*******************************************************************************
 *
 * Copyright (c) 2008, 2009 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
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
#define CPU_FREQUENCY 16000000UL

#define BOOST_NO_CONFIG

#include <stdio.h>
#include <stdlib.h>
#include "mw/common/ExtendedEvent.h"
#include "mw/attributes/filter/exists.h"
#include "mw/attributes/TTL.h"
using namespace famouso::mw::attributes;

#include "debug.h"

template< typename F, typename T>
bool filter_event(const F& f, T e) {
    bool b=f(e);
    if ( b )
        ::logging::log::emit() << "Filter matches" << ::logging::log::endl;
    else
        ::logging::log::emit() << "Filter does not match" << ::logging::log::endl;

    return b;
}
template<typename F>
void filter_tester(const F& f) {
    famouso::mw::ExtendedEvent<0, TTL<0> > e0(famouso::mw::Subject(0xf1));
    famouso::mw::ExtendedEvent<0, TTL<1> > e1(famouso::mw::Subject(0xf1));
    famouso::mw::ExtendedEvent<0, TTL<2> > e2(famouso::mw::Subject(0xf1));
    famouso::mw::ExtendedEvent<0, TTL<3> > e3(famouso::mw::Subject(0xf1));
    famouso::mw::ExtendedEvent<0 > e4(famouso::mw::Subject(0xf1));

    ::logging::log::emit() << "sizeof((f))="<< sizeof(f) << ::logging::log::endl;
    filter_event(f, e0);
    filter_event(f, e1);
    filter_event(f, e2);
    filter_event(f, e3);
    filter_event(f, e4);
}

#include "mw/attributes/filter/FilterExpression.h"
#include "mw/attributes/filter/RelationalOperatorFilter.h"

