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

#define LOGGING_DEFINE_EXTENDED_OUTPUT_TYPE
//#define LOGGING_DISABLE
#include "logging/logging.h"
using namespace ::logging;

// extensions start
/*! \brief define a simple ansi console colors struct
 *
 * The contained values correspond to ansi color definitions
 * However, the colors are only work on platforms that are
 * able to interpret these ansi escape sequences, like
 * [L|U]nix
 */
struct ConsoleColors {
    enum Colors {
        magenta = 35,
        bold = 1,
        reset = 0
    };
};

/*! \brief Defines the extended output type that is capable to interpret
 *         the colors and produce the correct escape sequences.
 */
template < typename Base>
struct CC : public Base {
    /*! \brief catch color type and produce the correct escape sequence
     */
    CC& operator << (const ConsoleColors::Colors l) {
        unsigned char tmp=Base::getBase();
        *this << ::logging::log::dec << "\033[0" << static_cast<unsigned short>(l) << 'm';
        Base::setBase(tmp);
        return *this;
    }

    /*! \brief forward all unknown types to the base output type for
     *         further processing.
     */
    template<typename T>
    CC& operator << (const T &t) {
        Base::operator<<(t);
        return *this;
    }
};
// extensions end
LOGGING_DEFINE_OUTPUT( CC< ::logging::LoggingType> )


int main(int, char**) {
    ::logging::log::emit() << "Hello World! with the logging framework"
                << ::logging::log::endl << ::logging::log::endl;

    // using the extension to colorize the output
    ::logging::log::emit() << ConsoleColors::magenta
                << "Combining console codes"
                << ConsoleColors::reset
                << ::logging::log::endl << ::logging::log::endl;

    ::logging::log::emit() << "and back to normal color mode"
                << ::logging::log::endl;

    return 0;
}
