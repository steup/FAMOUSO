/*******************************************************************************
 *
 * Copyright (c) 2010-2011 Philipp Werner <philipp.werner@st.ovgu.de>
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


#ifndef __MANAGEMENTLAYER_H_C1ACE57CB36BA9__
#define __MANAGEMENTLAYER_H_C1ACE57CB36BA9__

#include "debug.h"

#include "mw/common/Subject.h"
#include "mw/common/NodeID.h"
#include "mw/common/Event.h"
#include "mw/api/EventChannel.h"
#include "mw/api/detail/ChannelTrampoline.h"
#include "mw/el/ml/RealTimeSetResStateEvent.h"
#include "mw/el/ml/ChannelRequirementsEvent.h"
#include "object/Queue.h"
#include "mw/attributes/AttributeSet.h"


//#include "mw/afp/shared/hexdump.h"
//using famouso::mw::afp::shared::hexdump;

namespace famouso {
    namespace mw {
        namespace el {
            namespace ml {

                /*!
                 *  \brief  The management layer, an optional sublayer of the event layer
                 *
                 *  It is responisble for publishing event channel requirements to the
                 *  management channel, which are used e.g. for reservation of real time
                 *  channels. It also subscribes to the management channel for receiving
                 *  and provessing network scheduler response events, as the second part
                 *  of the reservation protocol.
                 *
                 *  \tparam LL  Lower (sub)layer type
                 *  \tparam EL  Event layer type
                 */
                template <class LL, class EL>
                class ManagementLayer : public LL {

                        /// Layer below the event layer (Abstract NL or Network Adapter)
                        typedef typename LL::LowerLayer BelowEL;

                        /// Event channel type
                        typedef famouso::mw::api::EventChannel<EL> EC;

                    public:
                        /*! \brief  short network representation of the subject
                         */
                        typedef typename LL::SNN SNN;

                        /*! \brief  Publish parameter set type
                         */
                        typedef typename LL::PublishParamSet PublishParamSet;

                        /*! \brief  Channel trampoline policy: trampoline needed
                         */
                        typedef famouso::mw::api::detail::ChannelTrampoline ChannelTrampolinePolicy;

                    protected:
                        struct config {
                            enum {
                                /*! \todo   It should be part of the famouso config, whether real time
                                 *          and forwarding support should be included.
                                 */
                                support_real_time = 1,
                                support_forwarding = 1,

                                publish_announcements = support_real_time,
                                publish_subscriptions = support_real_time | support_forwarding,

                                subscribe_man_chan = support_real_time,
                                announce_man_chan = publish_announcements | publish_subscriptions

                            };
                        };

                        /*! \brief  Subject of the management channel
                         */
                        static const char * const man_chan_subject;

                        /*! \brief  SNN of the management channel
                         */
                        SNN man_chan_snn;

                        /// Handles incoming event on management channel
                        void incoming_event(famouso::mw::api::SECCallBackData & event) {
                            if (config::support_real_time) {
                                // Is this event a real time reservation protocol event?
                                uint8_t event_type = ml::get_event_type(event.data, event.length);
                                if (event_type < ml::rt_res_event ||
                                    event_type > ml::rt_no_deliv_event)
                                    return;

                                ml::RealTimeSetResStateEvent reserv(event.data, event.length);
                                NodeID node_id;
                                ml::ChannelReservationData crd;

                                reserv.read_head(node_id);

                                // This event is intended for another node...?
                                if (!(node_id == getNodeID<void>()))
                                    return;

                                // Read rest of the event
                                if (event_type <= ml::rt_res_deliv_event) {
                                    reserv.read_reserv_tail(crd);
                                } else {
                                    reserv.read_no_reserv_tail(crd);
                                }
                                crd.event_type = event_type;

                                // Inform channel
                                EC * wanted = reinterpret_cast<EC*>(crd.lc_id.value());
                                EC * ec = static_cast<EC*>(LL::Publisher.select());
                                while (ec) {
                                    if (ec == wanted) {
                                        ec->set_real_time_reserv_state(&crd);
                                        break;
                                    }
                                    ec = static_cast<EC*>(ec->select());
                                }
                            }
                        }

                        /*!
                         *  \brief  Publish channel requirements event
                         *  \param  channel_list    List of the channels
                         *  \param  msg_type    Event type (either subscriptions_event or
                         *                      announcements_event)
                         *  \param  add_manchan Whether to include the management channel itself
                         *                      in the event
                         */
                        void publish_channel_info(const Queue & channel_list,
                                                  uint8_t msg_type,
                                                  bool add_manchan) {
                            EC * ec;
                            uint16_t length = ml::ChannelRequirementsEvent::header_size();

                            // Calculate length of event
                            {
                                ec = static_cast<EC*>(channel_list.select());
                                while (ec) {
                                    length += ml::ChannelRequirementsEvent::entry_size(ec->get_requirements(0));
                                    ec = static_cast<EC*>(ec->select());
                                }
                                if (add_manchan)
                                    length += attributes::AttributeSet<>::overallSize;
                            }

                            // Create event data
                            uint8_t data_buffer[length];
                            ml::ChannelRequirementsWriter crw;
                            crw.init(data_buffer, length);
                            {
                                // TODO: networkIDs
                                crw.write_head(msg_type, getNodeID<void>(), UID((uint64_t)0/* TODO */));

                                ec = static_cast<EC*>(channel_list.select());
                                while (ec) {
                                    crw.write_channel(*ec);
                                    ec = static_cast<EC*>(ec->select());
                                }

                                // Add management channel
                                if (add_manchan)
                                    crw.write_channel< attributes::AttributeSet<> >(man_chan_subject, UID((uint64_t)0));
                            }

                            // Publish event
                            Event e(man_chan_subject);
                            e.data = data_buffer;
                            e.length = length;
                            BelowEL::write(man_chan_snn, e);
                            static_cast<EL*>(this)->publish_local(e);
                        }

                    public:

                        /// Initialize this and lower layers
                        void init() {
                            // initialization of lower layers
                            LL::init();

                            // get the management event channel ready for use
                            if (config::announce_man_chan || config::subscribe_man_chan)
                                BelowEL::bind(man_chan_subject, man_chan_snn);

                            /// \todo Set up timer for periodic announcements/subscriptions publishing.
                        }



                        /// Publish all current announcements in one event on management channel
                        void publish_announcements() {
                            if (config::publish_announcements) {
                                publish_channel_info(LL::Publisher, ml::announcements_event, true);
                            }
                        }


                        /// Publish all current subscriptions in one event on management channel
                        void publish_subscriptions() {
                            if (config::publish_subscriptions) {
                                publish_channel_info(LL::Subscriber, ml::subscriptions_event,
                                                     config::subscribe_man_chan);
                            }
                        }

                        /// Announce event channel
                        void announce(EC & ec) {
                            LL::announce(ec);
                            // Problem: floods net in usual case of multiple consecutive announcements
                            /*! \todo publish_announcements() has to be called in a cyclic manner.
                                      The invocation period should be a configuration parameter.
                                      Idea to reduce announcement latency: reduce time to invocation
                                      here to a certain configured value, if it is not below that
                                      value */
                            publish_announcements();
                        }

                        /// Unannounce event channel
                        void unannounce(EC & ec) {
                            LL::unannounce(ec);
                            // Problem: floods net in usual case of multiple consecutive unannouncements
                            publish_announcements();
                        }

                        /// Subscribe event channel
                        void subscribe(EC & ec) {
                            LL::subscribe(ec);
                            publish_subscriptions();
                        }

                        /// Unsubscribe event channel
                        void unsubscribe(EC & ec) {
                            LL::unsubscribe(ec);
                            publish_subscriptions();
                        }

                        /// Publish event locally
                        void publish_local(const Event &e) {
                            if (config::subscribe_man_chan) {
                                if (e.subject == man_chan_subject)
                                    incoming_event(e);
                            }
                            LL::publish_local(e);
                        }

                        /// Fetch event
                        void fetch(famouso::mw::nl::DistinctNL *bnl = 0) {
                            if (config::subscribe_man_chan) {
                                // Only call lower layer fetch if it is no management event
                                if (LL::try_fetch_and_process(man_chan_subject, man_chan_snn, bnl))
                                    return;
                            }
                            // Lower layer handles events for normal channels
                            LL::fetch(bnl);
                        }
                };

                template <class LL, class EL>
                const char * const ManagementLayer<LL, EL>::man_chan_subject = "ManChan!";

            } // namespace ml
        } // namespace el
    } // namespace mw
} // namespace famouso

#endif // __MANAGEMENTLAYER_H_C1ACE57CB36BA9__

