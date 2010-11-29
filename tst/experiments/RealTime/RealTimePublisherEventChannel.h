/*******************************************************************************
 *
 * Copyright (c) 2010 Philipp Werner <philipp.werner@st.ovgu.de>
 *                    Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
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

#ifndef __REALTTIMEPUBLISHEREVENTCHANNEL_H_F5AE863048FD9B__
#define __REALTTIMEPUBLISHEREVENTCHANNEL_H_F5AE863048FD9B__

#include "debug.h"
#include "Dispatcher.h"
#include "Attributes.h"

#include "mw/common/Event.h"

using namespace famouso::mw;

// TODO: move down in inheritance stack
template <typename EC, typename Requirement>
class RealTimePublisherEventChannel : public EC {
        typedef RealTimePublisherEventChannel type;
    protected:
        // Period in us
        typedef attributes::Period<0> PeriodType;

        PeriodType::value_type get_period() {
            typedef typename Requirement::template find_ct<PeriodType>::type PT;
            return PT::value;
        }

    public:
        // logging helper
        struct RT {
            /*! \brief delivers the current %level of %logging */
            static ::logging::Level::levels level() {
                return ::logging::Level::user;
            }
            /*! \brief delivers the string reporting the current %level of %logging */
            static const char * desc() {
                return "[ RT PUB ] ";
            }
        };

        RealTimePublisherEventChannel(const Subject& subject) : EC(subject) {
            EC::mw_action_trampoline.template bind<type, &type::mw_action_impl>(this);
            delivering = false;
            event = 0;
        }

        void publish(const Event & event) {
            this->event = &event;

            // TODO: wider range (us)
            expire = TimeSource::current().add_usec(get_period());
            ::logging::log::emit<RT>()
                << "publish on " << ::logging::log::hex << static_cast<void*>(this)
                << " at " << Time::current() << " expiring at " << expire << "\n";
        }

    private:
        // Event-Daten + expire-time intern (TODO: interner puffer, double buffered?!?)
        const Event * event;
        // Setting initial expire to a time in past will prevent to deliver an uninitalized event buffer
        Time expire;

        // je netz: (slot-length), timer-handles, (RT-State)
        Task deliver_task;
        bool delivering;

        typedef typename EC::MWAction MWAction;

        uint16_t mw_action_impl(MWAction & mw_action) {
            if (mw_action.action == MWAction::get_requirements) {
                // Return requirement attribute set
                if (mw_action.buffer) {
                    new (mw_action.buffer) Requirement;
                }
                return Requirement::overallSize;
            }
            if (mw_action.action == MWAction::start_real_time_delivery) {
                // Start periodic delivery of published data
                const ChannelReservationData * rd = reinterpret_cast<const ChannelReservationData *>(mw_action.buffer);

                // Set RT status
                delivering = true;

                // Schedule periodic deliver task
                deliver_task.start.set(rd->tx_ready_time);
                deliver_task.period = get_period();
                deliver_task.function.bind<type, &type::deliver>(this);
                Dispatcher::instance().enqueue(deliver_task);

                // TODO: rd->tx_window_time for guard

                ::logging::log::emit<RT>()
                    << "start deliver: on "
                    << ::logging::log::hex << reinterpret_cast<uint64_t>(this)
                    << " at " << Time::current() << ": first deliver at "
                    << ::logging::log::dec << deliver_task.start << "\n";
                return 1;
            }
            return 0;
        }

        void deliver(/* NetworkID */) const {
            // publish_local bei voidNL?
            // bei "erstem" netz publish_local
            if (event) {
                if (TimeSource::in_future(expire)) {
                    ::logging::log::emit<RT>()
                        << "deliver: from "
                        << ::logging::log::hex << reinterpret_cast<uint64_t>(this)
                        << " at " << Time::current() << " expiring at " << expire << "\n";
                    EC::ech().template publish_local<typename EC::eventChannelHandler>(*event);
                } else {
                    ::logging::log::emit<RT>()
                        << "deliver: from "
                        << ::logging::log::hex << reinterpret_cast<uint64_t>(this)
                        << " at " << Time::current() << " EXPIRED at " << expire << "\n";
                }
            }
        }

};


#endif // __REALTTIMEPUBLISHEREVENTCHANNEL_H_F5AE863048FD9B__

