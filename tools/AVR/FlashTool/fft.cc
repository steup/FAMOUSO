/*******************************************************************************
 *
 * Copyright (c) 2010 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
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

// Redirect logging to stderr
#define LOGGING_DEFINE_EXTENDED_OUTPUT_TYPE
#include "logging/logging.h"
LOGGING_DEFINE_OUTPUT( ::logging::config::StdErrLogType )

#include "famouso_bindings.h"

#include <stdio.h>

using namespace famouso::mw;


const char * const default_channel_subject = "UPDATEuC";

int main(int argc, char ** argv) {
    ::logging::log::emit()
        << "FAMOUSO Flash Tool -> fft"
        << ::logging::log::endl
        << ::logging::log::endl;
    if (argc<2) {
        ::logging::log::emit() << "To few arguments" << ::logging::log::endl;
        ::logging::log::emit() << "use " << argv[0] <<" flash file.bin" << ::logging::log::endl;
        exit(-1);
    } else {
        famouso::init<famouso::config>();

        // Read data from stdin, however at the moment only 64kB are supported
        uint32_t buffered_size = 0xffffu;
        uint8_t  buffer[buffered_size];
        int32_t size = 0;

        size += fread(buffer, 1, buffered_size, stdin);

        // Announce
        famouso::config::PEC pec(default_channel_subject);
        pec.announce();

        // Publish
        Event event (pec.subject());
        event.data = buffer;
        // the length describes the flash page size of the at90can128 uC
        event.length = 256;
        while( size > 0 ) {
            ::logging::log::emit() << "still to transfer bytes = "<<size << ::logging::log::endl;
            pec.publish(event);
            event.data+=256;
            size-=256;
            if (size<256)
                event.length = size;
            usleep(100000);
        }

    }

    return 0;
}

