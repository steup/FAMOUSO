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

// for at90can128 with 16MHz CPU clock
#define CPU_FREQUENCY 16000000

//#define LOGGING_DISABLE
#include "logging/logging.h"
using namespace ::logging;

// logging levels can be disabled at compile time
//LOGGING_DISABLE_LEVEL(::logging::Error);
//LOGGING_DISABLE_LEVEL(::logging::Trace);
//LOGGING_DISABLE_LEVEL(::logging::Warning);
//LOGGING_DISABLE_LEVEL(::logging::Info);

struct Test {
    Test() {
        log::emit< ::logging::Info>() << "Test::Test()\n";
    }
    ~Test() {
        log::emit< ::logging::Info>() << "~Test::Test()\n";
    }
};

int main(int, char**) {
    log::emit() << "Hello World! with the logging framework" << log::endl << log::endl;
    log::emit() << "Print 15 in hexadecimal " << log::hex << 15 << log::endl;
    log::emit() << "Print 15 in decimal" << log::dec << 15 << log::endl;
    log::emit() << "Print 15 in octal " << log::oct << 15 << log::endl;
    log::emit() << "Print 15 in binary with a tab" << log::bin << log::tab << 15 << log::endl << log::endl;
    log::emit< ::logging::Error>() << "Logging an Error" << log::endl;
    log::emit< ::logging::Trace>() << "Logging a Trace" << log::endl;
    log::emit< ::logging::Warning>() << "Logging a Warning" << log::endl;
    log::emit< ::logging::Info>() << "Logging an Info" << log::endl;
    return 0;
}
