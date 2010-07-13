/*******************************************************************************
 *
 * Copyright (c) 2009-2010 Philipp Werner <philipp.werner@st.ovgu.de>
 *                         Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
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

#ifndef __AFPPUBLISHEREVENTCHANNEL_H_472698B5E7680A__
#define __AFPPUBLISHEREVENTCHANNEL_H_472698B5E7680A__


#include "mw/afp/Fragmenter.h"
#include "mw/afp/CommonPolicySelector.h"

#include "debug.h"
#include "mw/common/Event.h"

#include "assert/staticerror.h"


namespace famouso {
    namespace mw {
        namespace afp {


            using famouso::mw::Event;
            using famouso::mw::Subject;


            /*!
             * \brief Publisher Event Channel with fragmentation support in the application layer
             * \param PEC Publisher Event Channel
             * \param AFPFC AFP fragmentation config
             * \param mtu Maximum transmission unit (zero if not known at compile time)
             * \param EventType Event type to support publishing for (needs data, length and subject member variables)
             */
            template < class PEC, class AFPFC, uint32_t mtu, class EventType = Event >
            class AFPPublisherEventChannel : public PEC  {

                    /// Fragmenter config policies
                    typedef CommonPolicySelector<AFPFC> FCP;

                    /// Check if MTU fits to config
                    FAMOUSO_STATIC_ASSERT_ERROR(mtu == (typename FCP::SizeProp::flen_t)mtu,
                                                MTU_too_large_for_used_SizeProperties, ());

                    /// Maximum fragment size
                    enum { _mtu = mtu };

                    /// Fragment buffer
                    uint8_t _buffer[_mtu];

                public:

                    /*!
                     * \brief Constructor
                     * \param s Subject of the channel
                     */
                    AFPPublisherEventChannel(const Subject & s) :
                            PEC(s) {
                    }

                    /*!
                     * \brief Publish an event
                     * \param e Event to publish
                     * \return True if fragmentation was successful.
                     */
                    bool publish(const EventType & e) {
                        Event fragment_e(e.subject);
                        fragment_e.data = _buffer;

                        if (e.length != (typename FCP::SizeProp::elen_t) e.length) {
                            ::logging::log::emit< ::logging::Error>()
                                << PROGMEMSTRING("AFP: Cannot publish event... too large.")
                                << ::logging::log::endl;
                            return false;
                        }

                        afp::Fragmenter<AFPFC, _mtu> f(e.data, e.length);

                        if (f.error())
                            return false;

                        while ( (fragment_e.length = f.get_fragment(fragment_e.data)) ) {
                            PEC::publish(fragment_e);
                        }

                        return !f.error();
                    }
            };


            /*!
             * \brief Publisher Event Channel with fragmentation support in the application layer (MTU not known at compile time)
             * \param PEC Publisher Event Channel
             * \param EventType Event type to support publishing for (needs data, length and subject member variables)
             * \param AFPFC AFP fragmentation config
             */
            template <class PEC, class AFPFC, class EventType>
            class AFPPublisherEventChannel<PEC, AFPFC, 0, EventType> : public PEC  {

                    /// Fragmenter config policies
                    typedef CommonPolicySelector<AFPFC> FCP;

                    /// Allocator to use
                    typedef typename FCP::Allocator Allocator;

                    /// Maximum fragment size
                    uint32_t _mtu;

                    /// Fragment buffer
                    uint8_t * _buffer;

                public:

                    /*!
                     * \brief Constructor
                     * \param s Subject of the channel
                     * \param mtu MTU to use for fragmentation. Should be same for all publisher/subscriber of this subject.
                     */
                    AFPPublisherEventChannel(const Subject &s, uint16_t mtu)
                            : PEC(s), _mtu(mtu) {
                        _buffer = Allocator::alloc(mtu);
                        if (!_buffer) {
                            ::logging::log::emit< ::logging::Error>()
                                << PROGMEMSTRING("AFP: Out of memory")
                                << ::logging::log::endl;
                        }
                    }

                    /*!
                     * \brief Destructor
                     */
                    ~AFPPublisherEventChannel() {
                        if (_buffer)
                            Allocator::free(_buffer);
                    }

                    /*!
                     * \brief Publish an event
                     * \param e Event to publish
                     * \return True if fragmentation was successful.
                     */
                    bool publish(const EventType & e) {
                        if (!_buffer)
                            return false;

                        Event fragment_e(e.subject);
                        fragment_e.data = _buffer;

                        if (e.length != (typename FCP::SizeProp::elen_t) e.length) {
                            ::logging::log::emit< ::logging::Error>()
                                << PROGMEMSTRING("AFP: Cannot publish event... too large.")
                                << ::logging::log::endl;
                            return false;
                        }

                        afp::Fragmenter<AFPFC> f(e.data, e.length, _mtu);

                        if (f.error())
                            return false;

                        while ( (fragment_e.length = f.get_fragment(fragment_e.data)) ) {
                            PEC::publish(fragment_e);
                        }

                        return !f.error();
                    }
            };

        } // namespace afp
    } // namespace mw
} // namespace famouso


#endif // __AFPPUBLISHEREVENTCHANNEL_H_472698B5E7680A__

