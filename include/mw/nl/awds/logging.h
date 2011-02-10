/*******************************************************************************
 *
 * Copyright (c) 2008-2009 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
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

#ifndef LOGGING_H_
#define LOGGING_H_

#include "debug.h"
#include "mw/common/Subject.h"

namespace logging {

#if 0
    /*! \brief Operator to print a %Subject.
     *
     * \param out The output stream to print to.
     * \param s The Subject to print.
     * \return The output stream for chaining calls.
     */
    inline ::logging::loggingReturnType &operator <<(::logging::loggingReturnType &out, const famouso::mw::Subject &s) {
        for (uint8_t i = 0; i < 8; ++i) {
            uint8_t c = s.tab()[i];
            if ((c < 32) || (c > 126))
                c = '.'; // only printable characters
            out << c;
        }
        out << ' ' << '(' << log::hex << s.value() << ')';
        return out;
    }
#endif

    /*! \brief This class is intended to be used as a template argument for
     *         the logging::log::emit() function.
     *
     *         Prefixes the output with "[ AWDS ] " and enables reporting
     *         of the current logging level.
     */
    struct AWDS {
            /*! \brief delivers the current %level of %logging */
            static ::logging::Level::levels level() {
                return ::logging::Level::user;
            }
            /*! \brief delivers the string reporting the current %level of %logging */
            static const char * desc() {
                return "[ AWDS ] ";
            }
    };

    /*! \brief This class is intended to be used as a template argument for
         *         the logging::log::emit() function.
         *
         *         Prefixes the output with "[ ATTR ] " and enables reporting
         *         of the current logging level.
         */
        struct ATTR {
                /*! \brief delivers the current %level of %logging */
                static ::logging::Level::levels level() {
                    return ::logging::Level::user;
                }
                /*! \brief delivers the string reporting the current %level of %logging */
                static const char * desc() {
                    return "[ ATTR ] ";
                }
        };
}

namespace famouso {
    namespace mw {
        namespace nl {
            typedef struct ::logging::Error Error;
            typedef struct ::logging::AWDS AWDS;
            typedef ::logging::ATTR ATTR;
            typedef struct ::logging::log log;
        }
    }
}

#endif /* LOGGING_H_ */
