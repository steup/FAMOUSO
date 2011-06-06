/*******************************************************************************
 *
 * Copyright (c) 2008-2010 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
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

#ifndef __InterruptEnabler_h__
#define __InterruptEnabler_h__

#ifdef __AVR__
#include <avr/io.h>
#include <avr/interrupt.h>
    typedef uint8_t storage;
#else
    typedef uint32_t storage;
#endif

namespace famouso {
    namespace mw {
        namespace nl {
            namespace CAN {
                namespace etagBP {

                    /*! \brief InterruptEnabler implements a class enabling
                     *         interrupts for a certain scope.
                     *
                     *         That means as long as an object of
                     *         InterruptEnabler exists interrupts are on and
                     *         after the destruction of the object it is
                     *         restored to the former level.
                     */
                    class InterruptEnabler {
                            storage _ints;
                        public:
                            /*! \brief safe the current interrupt level and
                             *         permits interrupts
                             */
                            InterruptEnabler();

                            /*! \brief restores the former interrupt state
                             */
                            ~InterruptEnabler();

                            /*! \brief process has to be called by the blocking
                             *         protocol to avoid busy waiting, allowing
                             *         further progress
                             */
                            void process();
                    };

#ifdef __AVR__
                    inline InterruptEnabler::InterruptEnabler() : _ints(SREG) {
                        sei();
                    }

                    inline InterruptEnabler::~InterruptEnabler(){
                        SREG=_ints;
                    }

                    inline void InterruptEnabler::process() {
                        // nothing to do here, because interrupts are allowed
                        // and the core is driven that way on AVRs
                    }
#endif
                }
            } /* namespace CAN */
        } /* namespace nl */
    } /* namespace mw */
} /* namespace famouso */


#endif

