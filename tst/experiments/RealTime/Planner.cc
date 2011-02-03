/*******************************************************************************
 *
 * Copyright (c) 2010 Philipp Werner <philipp.werner@st.ovgu.de>
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


#define LOGGING_OUTPUT_FILE "log_Planner.txt"
#define LOGGING_DEFINE_EXTENDED_OUTPUT_TYPE
#define LOGGING_DEFINE_OWN_OUTPUT_TYPE
#include "RealTimeLogger.h"
#include "logging/logging.h"
LOGGING_DEFINE_OUTPUT( ::logging::OutputLevelSwitchDisabled< ::logging::OutputStream< ::logging::RTFileOutput > > )

#include "TFW.h"
typedef famouso::time::GlobalClock<famouso::time::ClockDriverGPOS> Clock;
FAMOUSO_TIME_SOURCE(Clock)

#include "mw/common/NodeID.h"
template <>
UID getNodeID<void>() {
    return UID("RTSchedN");
}


#include "SlotScheduler.h"
#include "NetworkParameters.h"

#include "mw/api/EventChannel.h"
#include "mw/api/PublisherEventChannel.h"
#include "mw/api/SubscriberEventChannel.h"

#include "devices/nic/can/SocketCAN/SocketCAN.h"
//#include "devices/nic/can/peak/PeakCAN.h"
#include "mw/nl/can/etagBP/Client.h"
#include "mw/nl/can/ccp/Client.h"
#include "mw/nl/CANNL.h"

//#include "mw/el/EventLayerClientStub.h"
#include "mw/nl/voidNL.h"
#include "mw/anl/AbstractNetworkLayer.h"
#include "mw/el/EventLayer.h"

#include "mw/common/Event.h"
#include "famouso.h"

#include "ManagementLayer.h"
#include "Attributes.h"

#include "mw/attributes/detail/AttributeSetProvider.h"
#include "mw/api/ExtendedEventChannel.h"
#include "EventDispatcher.h"
#include "NewEventLayer.h"


namespace famouso {
    class config {
            typedef device::nic::CAN::SocketCAN can;
            //typedef device::nic::CAN::PeakCAN can;
            typedef famouso::mw::nl::CAN::ccp::Client<can> ccpClient;
            typedef famouso::mw::nl::CAN::etagBP::Client<can> etagClient;
            typedef famouso::mw::nl::CANNL<can, ccpClient, etagClient> NL;
            //typedef famouso::mw::nl::voidNL NL;
            typedef famouso::mw::anl::AbstractNetworkLayer<NL> ANL;

            //typedef famouso::mw::el::EventLayerClientStub BaseEL;
        public:
            typedef NewEventLayer<ANL, ManagementLayer> EL;
            typedef famouso::mw::api::PublisherEventChannel<EL> PEC;
            typedef famouso::mw::api::SubscriberEventChannel<EL> SEC;
    };
}

#include "Dispatcher.h"

//#include "mw/afp/shared/hexdump.h"
//using famouso::mw::afp::shared::hexdump;


#include <stdio.h>

#include "mw/nl/awds/logging.h"


#include "RealTimePublisherEventChannel.h"


#include "mw/afp/shared/div_round_up.h"
using famouso::mw::afp::shared::div_round_up;


using namespace famouso::mw;


typedef attributes::Period<0>::value_type period_t;
typedef uint16_t elen_t;


struct RealTimePublisher {
    enum Status { empty, announced, /*planning,*/ scheduled, acked, reserved };

    Status status;
    UID node_id;
    /// Local channel ID
    uint64_t lc_id;
    Subject subject;                // only for verbose output

    // Needed for adjusting the announcement list
    bool unannounced;

    struct SlotInfoUSec {
        // Welcher Type
        uint32_t period;
        uint32_t length;
        uint32_t tx_window;       // relative to shift
        uint32_t shift;
    } slot_usec;

    struct SlotInfoASlot {
        unsigned long period;
        unsigned long length;
        unsigned long shift;
    } slot_aslot;

    void set_announced(const UID & node_id, uint64_t lc_id, const Subject & subj, period_t period_us, elen_t max_event_length_bytes, const NetworkParameters & net_param) {
        this->node_id = node_id;
        this->lc_id = lc_id;
        this->subject = subj;

        this->slot_usec.period = period_us;
        net_param.bytes_to_slot_params(max_event_length_bytes, this->slot_usec.length, this->slot_usec.tx_window);
        this->slot_usec.shift = 0;      // Calculated during reservation

        FAMOUSO_ASSERT(period_us % net_param.usec_per_aslot == 0);
        this->slot_aslot.period = period_us / net_param.usec_per_aslot;
        this->slot_aslot.length = div_round_up(this->slot_usec.length, net_param.usec_per_aslot);
        this->slot_aslot.shift = 0;     // Calculated during reservation

        this->unannounced = false;
        this->status = announced;
    }

    void set_scheduled(unsigned int shift_aslots, const NetworkParameters & net_param) {
        FAMOUSO_ASSERT(status == announced);

        this->slot_aslot.shift = shift_aslots;
        this->slot_usec.shift = shift_aslots * net_param.usec_per_aslot;

        this->status = scheduled;
    }


    RealTimePublisher() :
        status(empty)
    {
    }

    void log() {
        using namespace ::logging;
        log::emit()
            << "- RT Publisher " << log::hex << lc_id << ": subject " << subject
            << ", node_id " << node_id
            << ";\tSlot period " << log::dec << slot_aslot.period << " (" << slot_usec.period << " us), "
            << "length " << slot_aslot.length << " (" << slot_usec.length << " us), "
            << "shift " << slot_aslot.shift << " (" << slot_usec.shift << " us)"
            << ::logging::log::endl;
    }

};

#include "case/Delegate.h"
using namespace famouso;

class RealTimeAnnouncements : public std::list<RealTimePublisher> {
    public:

        void begin_update(const UID & node_id) {
            // Mark all publisher of this node for unannouncement
            iterator it = begin();
            while (it != end()) {
                RealTimePublisher & pub = *it;
                if (pub.node_id == node_id) {
                    pub.unannounced = true;
                }
                ++it;
            }
        }

        // Update step: search publisher matching the node_id and lc_id, if found return 0 otherwise return new item for initialization
        RealTimePublisher * update(const UID & node_id, uint64_t lc_id) {
            iterator it = begin();
            while (it != end()) {
                RealTimePublisher & pub = *it;
                if (pub.node_id == node_id && pub.lc_id == lc_id) {
                    pub.unannounced = false;
                    return 0;
                }
                ++it;
            }

            push_back(RealTimePublisher());
            return &back();
        }

        // Return whether to remove
        typedef util::Delegate<const RealTimePublisher &, bool> UnannounceCallback;

        void end_update(const UID & node_id, const UnannounceCallback & unannounce_callback) {
            iterator it = begin();
            while (it != end()) {
                RealTimePublisher & pub = *it;
                if (pub.node_id == node_id && pub.unannounced) {
                    if (unannounce_callback(pub)) {
                        it = erase(it);
                        continue;
                    }
                }
                ++it;
            }
        }

        void log_publisher() {
            using namespace ::logging;
            log::emit() << "Real time publisher:\n";
            iterator it = begin();
            while (it != end()) {
                RealTimePublisher & pub = *it;
                pub.log();
                ++it;
            }
        }
};





class SingleNetworkSchedule {
        uint64_t cycle_time;
        uint64_t cycle_start;

    public:
        SingleNetworkSchedule() : cycle_time(0), cycle_start(0) {
        }

        // NetworkID
        RealTimeAnnouncements rt_announcements;
        //Subscriptions subscriptions;
        SlotScheduler slot_scheduler;
        NetworkParameters net_params;

        uint64_t get_next_cycle_start() {
            uint64_t curr = TimeSource::current().get();
            if (curr >= cycle_start) {
                // cycle_start in past -> calculate new
                uint64_t dt = curr - cycle_start;
                uint64_t rounds = dt / cycle_time;
                cycle_start += (rounds + 1) * cycle_time;
            }
            return cycle_start;
        }

        void set_cycle_time(uint64_t duration) {
            // Set cycle_time and update cycle_start
            cycle_time = duration;
            // TODO FIXME: bei mehreren changes hintereinander fehlerhaft!?!
            get_next_cycle_start();
        }
};


// TODO: Multi-Netz -> Erkennen von unvollständigen Routen

// Single-Net
// PEC, SEC: BE-Channel
template <class PEC, class SEC>
class RTNetPlanner {

        typedef RTNetPlanner ThisType;

        // Largest supported range of values
        typedef attributes::Period<0> PeriodType;
        typedef MaxEventSize<0> MaxEventSizeType;
        typedef attributes::AttributeSet<> AttrSet;

        //PEC reservation_channel;
        famouso::config::PEC man_chan_pub;
        famouso::config::SEC man_chan_sub;

        SingleNetworkSchedule net;


        void print_pub_info(const Subject & subject, uint64_t lc_id, const AttrSet * req_attr) {
            ::logging::log::emit() << "\tpublisher : subject " << subject
                                   << ", this " << ::logging::log::hex << lc_id
                                   << ", Attribute:"
                                   << ::logging::log::endl;
            //hexdump((uint8_t*)req_attr, req_attr->length());

            const PeriodType * period_p = req_attr->template find_rt<PeriodType>();
            if (period_p) {
                period_t period = period_p->getValue();
                ::logging::log::emit() << "\t\tperiod " << ::logging::log::dec
                                       << period
                                       << ::logging::log::endl;
            }

            const MaxEventSizeType * mes_p = req_attr->template find_rt<MaxEventSizeType>();
            if (mes_p) {
                elen_t max_event_length = mes_p->getValue();
                ::logging::log::emit() << "\t\tmax_event_size " << ::logging::log::dec
                                       << max_event_length
                                       << ::logging::log::endl;
            }

            const RealTime * rt_p = req_attr->template find_rt<RealTime>();
            if (rt_p) {
                ::logging::log::emit() << "\t\tRealTime!"
                                       << ::logging::log::endl;
            }
        }

        void process_announcements(const Event & event) {
            el::manchan::ChannelRequirementsReader crr(event.data, event.length);

            uint8_t msg_type;
            UID node_id;
            // TODO: networkIDs
            crr.read_head(msg_type, node_id);

            ::logging::log::emit() << "Announcements from node " << node_id
                                   << ::logging::log::endl;

            // Starts update of real time publisher list
            net.rt_announcements.begin_update(node_id);

            while (crr.further_channel()) {
                Subject subject; uint64_t lc_id; const AttrSet * req_attr;
                crr.read_channel(subject, lc_id, req_attr);

                print_pub_info(subject, lc_id, req_attr);

                // Extract attributes
                const RealTime * rt_p = req_attr->template find_rt<const RealTime>();
                if (rt_p) {
                    // Real Time announcement
                    const PeriodType * period_p = req_attr->template find_rt<PeriodType>();
                    const MaxEventSizeType * mes_p = req_attr->template find_rt<MaxEventSizeType>();

                    if (period_p && mes_p) {

                        // Assumes no changes in known announcements... returns only new publisher (uninitialized)
                        RealTimePublisher * rt_pub = net.rt_announcements.update(node_id, lc_id);

                        if (rt_pub) {
                            // New real time announcement
                            period_t period_us = period_p->getValue();
                            elen_t max_event_length_bytes = mes_p->getValue();

                            // Fill publisher data record
                            rt_pub->set_announced(node_id, lc_id, subject, period_us, max_event_length_bytes, net.net_params);

                            // Try to reserve slots
                            unsigned int aslot_shift = 0;
                            ::logging::log::emit() << "Newly announced channel: trying to reserve real time network resources... ";
                            if (net.slot_scheduler.reserve(rt_pub->slot_aslot.period, rt_pub->slot_aslot.length, aslot_shift)) {
                                ::logging::log::emit() << "success!" << ::logging::log::endl;
                                net.set_cycle_time(net.slot_scheduler.cycle_length() * net.net_params.usec_per_aslot);
                                rt_pub->set_scheduled(aslot_shift, net.net_params);
                                // TODO: if neccessary, send free_slots to BE-Poller and wait for ACK before sending reservation (only once per announcements msg?)
                                // wait for subscribers?
                                // Send reservations in one msg
                                {
                                    // Send reservation for this channel
                                    ml::ChannelReservationData crd;
                                    crd.lc_id = lc_id;
                                    crd.tx_ready_time = net.get_next_cycle_start() + rt_pub->slot_usec.shift;
                                    crd.tx_window_time = rt_pub->slot_usec.tx_window;

                                    uint16_t len = manchan::Reservation::size(1);
                                    uint8_t buffer[len];
                                    manchan::ReservationWriter rw(buffer, len);
                                    rw.write_head(node_id);
                                    rw.write_channel(crd);

                                    Event e(man_chan_pub.subject());
                                    e.data = buffer;
                                    e.length = len;
                                    man_chan_pub.publish(e);
                                }

                            } else {
                                // TODO: set rt_pub status to rejected or init it another way
                                ::logging::log::emit() << "failure!" << ::logging::log::endl;
                            }
                            // TODO: Output status (rt_announcements)
                            net.slot_scheduler.log_free_list();
                        }

                    } else {
                        ::logging::log::emit() << "Essential real time attribute missing: ignoring announcement"
                                               << ::logging::log::endl;
                    }
                }
            }

            net.rt_announcements.log_publisher();

            RealTimeAnnouncements::UnannounceCallback ucb;
            ucb.bind<ThisType, &ThisType::unannounce>(this);
            net.rt_announcements.end_update(node_id, ucb);
        }

        bool unannounce(const RealTimePublisher & pub) {
            ::logging::log::emit() << "Unannouncement of real time channel " << pub.subject
                                   << ::logging::log::endl;
            net.slot_scheduler.free(pub.slot_aslot.period, pub.slot_aslot.length, pub.slot_aslot.shift);
            net.slot_scheduler.log_free_list();
            return true;
        }

        void incoming_management_event(famouso::mw::api::SECCallBackData & event) {

            uint8_t msg_type = *event.data;
            if (msg_type == manchan::announce_msg)
                process_announcements(event);
            /*if (msg_type != manchan::subscribe_msg)*/

            return;
        }


    public:

        RTNetPlanner() :
            man_chan_pub("ManChan!"),
            man_chan_sub("ManChan!")
        {
            man_chan_sub.callback.template bind<ThisType, & ThisType::incoming_management_event>(this);
            man_chan_sub.subscribe();
            man_chan_pub.announce();
        }

};






typedef famouso::mw::attributes::detail::SetProvider<
         famouso::mw::attributes::Period<500000>,
         MaxEventSize<100>,
         RealTime
        >::attrSet rt_req;

class PEC2 : public RealTimePublisherEventChannel<famouso::config::PEC, rt_req> {
        Task pub_task;
        Event event;
    public:
        typedef RealTimePublisherEventChannel<famouso::config::PEC, rt_req> Base;

        PEC2() :
            RealTimePublisherEventChannel<famouso::config::PEC, rt_req>("rt__subj"),
            event(subject())
        {
            pub_task.start = TimeSource::current().add_usec(500000);
            pub_task.period = get_period();
            pub_task.function.bind<PEC2, &PEC2::publish_task>(this);
            Dispatcher::instance().enqueue(pub_task);
        }

        void publish_task() {
            event.data = (uint8_t *)"from_planner";
            event.length = 12;
            publish(event);
        }
};


void subscriber_callback(const famouso::mw::api::SECCallBackData& event) {
    ::logging::log::emit() << "callback: subject " << event.subject
                           << ", length " << ::logging::log::dec << event.length
                           << ", data " << event.data
                           << ", time " << famouso::TimeSource::current()
                           << logging::log::endl;
}

int main(int argc, char ** argv) {
    famouso::init<famouso::config>();

    // can be integrated into famouso init
    famouso::config::SEC time_chan("TimeSync");
    typedef famouso::TimeSource::clock_type Clock;
    time_chan.callback.bind<Clock, &Clock::tick>(&famouso::TimeSource::instance());
    time_chan.subscribe();

    while (famouso::TimeSource::out_of_sync()) {
        usleep(1000);
    }
    printf("Clock in sync\n");

    RTNetPlanner<famouso::config::PEC, famouso::config::SEC> planner;

    famouso::config::SEC sec1("rt__subj");
    sec1.callback.bind<&subscriber_callback>();
    sec1.subscribe();

    famouso::config::PEC pec1("abcdefgh");
    pec1.announce();

//    {
    PEC2 pec2;
    pec2.announce();
//    }

    printf("Start dispatcher\n");
    Dispatcher::instance().run();

    return 0;
}

