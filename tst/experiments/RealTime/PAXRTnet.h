/*******************************************************************************
 *
 * Copyright (c) 2008-2010 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
 *                    2011 Philipp Werner <philipp.werner@st.ovgu.de>
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

#ifndef __PAXRTnet_h__
#define __PAXRTnet_h__

#include "object/SynchronizedBoundedBuffer.h"

namespace device {
    namespace nic {
        namespace ethernet {

            /*! \brief  PAXRTnet is a modification of PAXCAN for use with EthernetNL.
             *
             *  \tparam the Driver is the underlying Ethernet driver.
             */
            template <class Driver>
            class PAXRTnet : public Driver {
                public:

                    PAXRTnet() : sbb(1), rcv_to_use(0) {}

                    /*! \brief initialize the subclasses */
                    void init() {
                        Driver::init();
                    }

                    /*! \brief receives a message
                     *
                     *  \param[out] mob is filled with the arrived message if there
                     *              was one
                     *
                     *  \return true if a message was arrived
                     *  \return false otherwise
                     */
                    typename Driver::MOB * receive() {
                        typename Driver::MOB * m = 0;
                        if (sbb.is_not_empty()) {
                            sbb.pop_back(&m);
                        }
                        return m;
                    }

                    /*! \brief receives a message in a blocking manner
                     *
                     *  \param[out] mob is filled with the arrived message if there
                     *              was one
                     */
                    typename Driver::MOB * receive_blocking() {
                        typename Driver::MOB * m;
                        sbb.pop_back(&m);
                        return m;
                    }

                    /*! \brief transmit a message in a blocking manner
                     *
                     *  \param[in] mob is message that will be delivered
                     */
                    void transmit(typename Driver::MOB &mob) {
                        Driver::write(mob);
                    }

                private:
                    /*! \brief Used for storing messages in a thread safe manner */
                    object::SynchronizedBoundedBuffer<typename Driver::MOB *>  sbb;

                    // this is a workaround for concurrency problem
                    // dequeueing down not mean the buffer can be used again...
                    // TODO: better solution
                    enum { buf_count = 50 }; // assuming more than 50 buffer are not read before the data of the first is copied by AFP or the subscriber callback
                    typename Driver::MOB rcv_mob[buf_count];
                    int rcv_to_use;

                protected:
                    /*! \brief test if there is a message arrived and signalise the
                     *         calling context, whether the interrupt condidtion is
                     *         true or false
                     *
                     *  \return true if a message was arrived
                     *  \return false otherwise
                     */
                    bool interrupt_condition () {
                        if (Driver::read(rcv_mob[rcv_to_use]) == true ) {
                            ::logging::log::emit() << "PAX: interrupt_condition() " << rcv_to_use <<  "\n";
                            sbb.push_front(&rcv_mob[rcv_to_use]);
                            rcv_to_use++;
                            rcv_to_use %= buf_count;
                            return true;
                        }
                        return false;
                    }
            };

        } /* namespace ethernet */
    } /* namespace nic */
} /* namespace device */

#endif

