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

#ifndef __EventChannelHandler_h__
#define __EventChannelHandler_h__

namespace famouso {
    namespace mw {
        namespace el {

            /*! \brief The event channel handler is a singleton of the event layer
             *
             *  \tparam EL the event layer is a configurable component see prerequisites.
             *
             *  \pre    The type of template parameters have to be
             *          an configured famouso::mw::el::EventLayer
             */
            template < class EL >
            class EventChannelHandler : public EL {

                    /*! \brief placement new operator to allow a constructor call
                     *         on pre-allocated memory
                     */
                    void* operator new(size_t, void* __p) {
                        return __p;
                    }

                public:
                    /*! \brief get the local event channel handler object instance
                     */
                    static inline EL& ech() {
                        // definition of the event channel handler
                        static uint8_t _ech[sizeof(EL)];
                        return reinterpret_cast<EL&>(_ech);
                    }


                    /*! \brief  creates the ech object and calls the constructor
                     */
                    static void create() {
                        // late/defered Constructor call on pre-allocated memory
                        // using the placement new of C++
                        new(&ech()) EventChannelHandler;
                    }

                    /*! \brief  initialize the middleware core by calling the
                     *          init function of the event layer
                     */
                    static void init() {
                        ech().init();
                    }

            };

        } // namespace el
    } // namespace mw
} // namespace famouso

#endif /* __EventChannelHandler_h__ */

