/*******************************************************************************
 *
 * Copyright (c) 2008-2010 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
 *               2009-2010 Michael Kriese <kriese@cs.uni-magdeburg.de>
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

#ifndef _config_h_
#define _config_h_

#include "mw/el/EventLayerClientStub.h"
#include "mw/api/EventChannel.h"
#include "mw/api/PublisherEventChannel.h"
#include "mw/api/SubscriberEventChannel.h"

#include "mw/common/Event.h"
#include "famouso.h"

#include <list>
#include "boost/shared_ptr.hpp"

namespace famouso {
    class config {
        public:
            typedef famouso::mw::el::EventLayerClientStub EL;
            typedef famouso::mw::api::PublisherEventChannel  < EL > PEC;
            typedef famouso::mw::api::SubscriberEventChannel < EL > SEC;
    };
}

namespace logging {

    inline ::logging::loggingReturnType &operator <<(::logging::loggingReturnType &out, const famouso::mw::Subject &s) {
        for (uint8_t i = 0; i < 8; ++i) {
            uint8_t c = s.tab()[i];
            if ((c < 32) || (c > 126))
                c = '.'; // only printable characters
            out << c;
        }
        out << " (" << log::hex << s.value() << ")";
        return out;
    }
}

#define SUBJECTS_MAX 10
#define SUBJECT(x) { 'T', 'E', 'S', 'T','_','0', '0', (x+'0') }

#endif /* _config_h_ */
