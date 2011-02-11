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

#ifndef __NetworkAdapter_h__
#define __NetworkAdapter_h__

#include "mw/nl/DistinctNL.h"
#include "mw/el/EventLayer.h"
#include "mw/api/SubscriberEventChannel.h"
#include "mw/gwl/GatewayEventChannel.h"
#include "mw/common/Event.h"

namespace famouso {
    namespace mw {
        namespace nal {

            /*! \brief The network adapter supports plugging of more than one network layer
             *
             *  \tparam ANL_A first sub network
             *  \tparam ANL_B second sub network
             *
             *  \pre The type of template parameters needs to be of the type famouso::mw::anl::AbstractNetworkLayer
             *
             */
            template < class ANL_A, class ANL_B >
            class NetworkAdapter {
                    typedef NetworkAdapter< ANL_A, ANL_B> this_type;

                    ANL_A  _ANL_A;
                    ANL_B  _ANL_B;
                    famouso::mw::nl::DistinctNL *_bnl;

                protected:

                    /*! \brief The struct contains the short network name representation of the subject
                     *         of an event for both sub networks.
                     */
                    struct SNN {
                        typename ANL_A::SNN SNN_A;
                        typename ANL_B::SNN SNN_B;
                    };

                    /*! \brief Initalizes the sub networks.
                     */
                    void init() {
                        _ANL_A.init();
                        _ANL_B.init();
                    }

                public:
                    /*! \brief Publish to a specific network is a support functionality for
                     *         higher layer e.g the famouso::mw::gwl::Gateway.
                     *
                     *  \param[in] snn is the specific short network name that is in this case the struct SNN.
                     *             The struct contains the short network name representation of the subject
                     *             of the event for both sub networks.
                     *  \param[in] e the Event that should be selctive published.
                     *  \param[in] bnl the sub network in that the event \e e will be published.
                     */
                    void publish_to_network_with_id(const SNN &snn, const Event &e, const famouso::mw::nl::DistinctNL *bnl) {
                        if (_ANL_A.id() == bnl)
                            _ANL_A.publish(snn.SNN_A , e);
                        else
                            _ANL_B.publish(snn.SNN_B , e);
                    }

                    /*! \brief Allows for checking if source of event and subscription network matches.
                     *
                     *         This functionality is also provided for higher layers e.g. the
                     *         famouso::mw::gwl::Gateway.
                     *
                     *  \param[in] b network-ID that needs to be checked
                     *  \return \li \b true if \e b and the source of the occurred event are equal
                     *              or if the subscription comes from local because this leads to
                     *              publishing on all sub networks.
                     *          \li \b false otherwise
                     */
                    const bool event_from_network_with_id(const famouso::mw::nl::DistinctNL *b) const {
                        if ((!_bnl)  || (_bnl == b))
                            return true;
                        else
                            return false;
                    }

                    /*! \brief Allows for geting the source network ID of an arised event.
                     *
                     *         This functionality is provided for higher layers e.g. the
                     *         famouso::mw::gwl::Gateway.
                     *
                     *  \return the network-ID of the arised event.
                     */
                    const famouso::mw::nl::DistinctNL * const get_network_id() const {
                        return _bnl;
                    }

                protected:

                    /*! \brief Announces a subject on all sub networks.
                     *
                     *  \param[in]  s the subject that is announced.
                     *  \param[out] snn is the specific short network name that is in this case the struct SNN.
                     *              After the call the struct contains the short network names of the announced
                     *              subject.
                     */
                    void announce(const Subject &s, SNN &snn) {
                        _ANL_A.announce(s, snn.SNN_A);
                        _ANL_B.announce(s, snn.SNN_B);
                    }

                    /*! \brief Publishes an event on all sub networks.
                     *
                     *  \param[in]  snn is the specific short network name that is in this case the struct SNN.
                     *              The struct contains the short network name representation of the subject
                     *              of the event for both sub networks.
                     *  \param[in]  e the event that is published.
                     */
                    void publish(const SNN &snn, const Event &e) {
                        _ANL_A.publish(snn.SNN_A , e);
                        _ANL_B.publish(snn.SNN_B , e);
                    }

                    /*! \brief Subscribes a subject on all sub networks.
                     *
                     *  \param[in]  s the subject that is subscribed.
                     *  \param[out] snn is the specific short network name that is in this case the struct SNN.
                     *              After the call the struct contains the short network names of the subsrcibed
                     *              subject.
                     */
                    void subscribe(const Subject &s, SNN &snn) {
                        _ANL_A.subscribe(s, snn.SNN_A);
                        _ANL_B.subscribe(s, snn.SNN_B);
                    }

                    /*! \brief Traverses a specific sub network for a short
                     *         network name and receives an event from that
                     *         specific subnetwork.
                     *
                     *  \param[in]  snn is the specific short network name that is in this case the struct SNN.
                     *              The struct contains the short network name representation of the subject
                     *              of the event for both sub networks.
                     *  \param[out] e the event that is filled with the received event
                     *  \param[in]  bnl the network from where an event fetching was requested.
                     *  \return \li \b -1 if \e snn and the snn of the last arosen packet are different
                     *          \li \b 0 if they are equal but there is no complete event to fetch
                     *          \li \b 1 if they are equal and \e e contains a complete event
                     */
                    int8_t fetch(const SNN &snn, Event &e, const famouso::mw::nl::DistinctNL *bnl) {
                        /*! \todo fetching of stacked gateway are not implemented at this stage and
                         *        time triggered fetching is also not supported. */
                        if (_ANL_A.id() == bnl)
                            return _ANL_A.fetch(snn.SNN_A, e, bnl);
                        else
                            return _ANL_B.fetch(snn.SNN_B, e, bnl);
                    }

                    /*! \brief Is called by the higher layer to signalise that
                     *         an event processing request was arised.
                     *
                     *  \param[in]  bnl the sub network-ID from where the request came.
                     *  \return     Returns whether to continue with event processing.
                     */
                    bool event_process_request(famouso::mw::nl::DistinctNL * const bnl) {
                        _bnl = bnl;
                        if (_ANL_A.id() == bnl)
                            return _ANL_A.event_process_request(bnl);
                        else
                            return _ANL_B.event_process_request(bnl);
                    }

                    /*! \brief Is called by the higher layer to signalise that
                     *         the event is processed now.
                     */
                    void event_processed() {
                        if (_ANL_A.id() == _bnl)
                            _ANL_A.event_processed();
                        else
                            _ANL_B.event_processed();
                        _bnl = 0;
                    }
            };
        } // namespace nal
    } // namespace mw
} //namespace famouso

#endif /* __NetworkAdapter_h__ */
