/*******************************************************************************
 *
 * Copyright (c) 2011 Philipp Werner <philipp.werner@st.ovgu.de>
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

#ifndef __XENOMAICLOCK_H_232A0EA7427B5B__
#define __XENOMAICLOCK_H_232A0EA7427B5B__

#include "TimestampingXenomaiRTCAN.h"
#include "timefw/Time.h"
#include "mw/common/Event.h"

//Gergeleit/Streich sync
    // Accurancy needed for sync
    // Consecutive sync ticks with acc < wanted_acc for sync
    // interface: nanosec
    // internal: nanosec
    // Only works with TimestampingXenomaiRTCANDriver
    template <class ClockDriver, uint64_t accuracy_us, unsigned int stability_count = 3>
    class GlobalXenomaiClock : public ClockDriver {
            class GergeleitStrechPIDriftCorrector {
                    enum { NUM_OF_TIMESTAMPS = 4 };

                    uint64_t server_timestamps [NUM_OF_TIMESTAMPS];
                    uint64_t client_timestamps [NUM_OF_TIMESTAMPS];
                    double clock_drifts [NUM_OF_TIMESTAMPS];

                    uint64_t & server_timestamp(unsigned int i) {
                        return server_timestamps[i % NUM_OF_TIMESTAMPS];
                    }

                    uint64_t & client_timestamp(unsigned int i) {
                        return client_timestamps[i % NUM_OF_TIMESTAMPS];
                    }

                    double & drift(unsigned int i) {
                        return clock_drifts[i % NUM_OF_TIMESTAMPS];
                    }

                    // globale Zeit zu letztem Sync-Zeitpunkt
                    double glob_last_sync_time;

                    // lokale Zeit zu letztem Sync-Zeitpunkt
                    double local_last_sync_time;

                    // Timestamp index
                    unsigned int glob_i;

                    double integral;
                    double accuracy;

                    uint64_t wanted_abs_acc;
                    unsigned int wanted_stability_count;
                    unsigned int curr_stability_count;

                    void update_sync_status() {
                        uint64_t abs_acc = accuracy < 0 ? -accuracy : accuracy;
                        if (abs_acc < wanted_abs_acc) {
                            curr_stability_count++;
                        } else {
                            curr_stability_count = 0;
                        }
                    }

                public:
                    GergeleitStrechPIDriftCorrector(uint64_t wanted_acc, unsigned int wanted_stab_count) :
                            glob_last_sync_time(0),
                            local_last_sync_time(0),
                            glob_i(0),
                            integral(0),
                            accuracy(1e30),
                            wanted_abs_acc(wanted_acc),
                            wanted_stability_count(wanted_stab_count),
                            curr_stability_count(0) {
                    }

                    bool out_of_sync() {
                        return curr_stability_count < wanted_stability_count;
                    }

                    double local_to_global(double local_time) {
#if 0
                        ::logging::log::emit() << "local_to_global:" <<
                            "\n\tglob_last_sync_time  = " << (uint64_t)glob_last_sync_time <<
                            "\n\tlocal_time           = " << (uint64_t)local_time <<
                            "\n\tlocal_last_sync_time = " << (uint64_t)local_last_sync_time <<
                            "\n\tdrift                = " << static_cast<uint64_t>(drift(glob_i-1) * 1000000000.0) << "e-9\n";
                        double out =  glob_last_sync_time + (local_time - local_last_sync_time) * drift(glob_i-1);
                        ::logging::log::emit() << "glob_last_sync_time + (local_time - local_last_sync_time) * drift(glob_i-1) = " <<
                            static_cast<uint64_t>(out) << "\n";
                        return out;
#else
                        return glob_last_sync_time +
                               (local_time - local_last_sync_time) * drift(glob_i-1);
#endif
                    }

                    double global_to_local(double global_time) {
                        return (global_time - glob_last_sync_time) / drift(glob_i-1) +
                               local_last_sync_time;
                    }

                    void process_sync(uint64_t server_ts, uint64_t client_ts) {
                        /*
                        ::logging::log::emit() << "[SYNC] server_ts "
                            << server_ts << " ns, client_ts "
                            << client_ts << " ns\n";
                            */
                        server_timestamp(glob_i) = server_ts;
                        client_timestamp(glob_i + 1) = client_ts;
                        if (glob_i < 3) {
                            // Init
                            drift(glob_i) = 1.0;
                            local_last_sync_time = client_timestamp(glob_i);
                            glob_last_sync_time = server_timestamp(glob_i);
                        } else {
                            // Update local drift
                            glob_last_sync_time = local_to_global(client_timestamp(glob_i));
                            local_last_sync_time = client_timestamp(glob_i);
                            /*
                            ::logging::log::emit() << "accur = " <<
                                "" << (uint64_t)glob_last_sync_time <<
                                " - " << (uint64_t)server_timestamp(glob_i);
                                */
                            accuracy = glob_last_sync_time - (double)server_timestamp(glob_i);
                            /*
                            ::logging::log::emit() << " = " <<
                                "" << (int64_t)accuracy << '\n';
                                */
                            integral += accuracy;
                            double cycle_time = (double)(server_timestamp(glob_i) - server_timestamp(glob_i-1));

                            /*
                            drift(glob_i) = 0.4 * drift(glob_i-1) +
                                            0.6 * ((cycle_time - accuracy - 0.5 * integral) / cycle_time);
                                            // 2511 656
                                            // 2199 585
                                            */
                            /*
                            drift(glob_i) = 0.4 * drift(glob_i-1) +
                                            0.6 * ((cycle_time - accuracy - 0.25 * integral) / cycle_time);
                                            // 2179 586
                                            // 2180 572
                                            // 1524 547
                                            */
                            /*
                            drift(glob_i) = 0.4 * drift(glob_i-1) +
                                            0.6 * ((cycle_time - accuracy - 0.1 * integral) / cycle_time);
                                            // 1826 518
                                            // 1562 486
                                            */
                            drift(glob_i) = 0.4 * drift(glob_i-1) +
                                            0.6 * ((cycle_time - accuracy - 0.05 * integral) / cycle_time);
                                            // 1722 433
                                            // 1496 488

                            /*
                            ::logging::log::emit() << "###### drift "
                                << static_cast<int64_t>(drift(glob_i) * 1000000000.0) << "e-9\n";
                                */

                            update_sync_status();
#ifdef CLOCK_ACCURACY_OUTPUT
#ifdef RT_TEST_STATISTICS
                            /*
                            ::logging::log::emit() << "[CLK t[us] a[ns]] "
                            */
                            ::logging::log::emit() << "[CLK] "
                                << ::logging::log::dec
                                << server_timestamp(glob_i)
                                << ' ' << static_cast<int64_t>(accuracy) << '\n';
#else
                            ::logging::log::emit() << "[SYNC] at "
                                << ::logging::log::dec
                                << timefw::Time::nsec(server_timestamp(glob_i))
                                << " acc " << static_cast<int64_t>(accuracy) << " ns\n";
#endif
#endif
#ifdef CLOCK_ACCURACY_TEST
                            static double max_acc = 0;
                            static double sum_acc = 0;
                            if (glob_i > 100) {
                                double abs_acc = accuracy < 0 ? -accuracy : accuracy;
                                sum_acc += abs_acc;
                                if (abs_acc > max_acc)
                                    max_acc = abs_acc;
                                ::logging::log::emit() << "[SYNC] max_acc " << static_cast<int64_t>(max_acc) << " ns, avg_acc "<<  static_cast<int64_t>(sum_acc / (double)(glob_i - 100)) << " ns, i "<< glob_i << "\n";
                                if (glob_i > 300)
                                    exit(0);
                            }
#endif

                        }
                        glob_i++;
                    }
            };
            GergeleitStrechPIDriftCorrector impl;

            timefw::Time local_to_global(const timefw::Time & local_time) {
                return timefw::Time::nsec(impl.local_to_global(local_time.get_nsec()));
            }

            timefw::Time global_to_local(const timefw::Time & global_time) {
                return timefw::Time::nsec(impl.global_to_local(global_time.get_nsec()));
            }

        public:
            GlobalXenomaiClock() :
                impl(accuracy_us*1000, stability_count) {
            }

            void sync_event(const famouso::mw::Event & e) {
                FAMOUSO_ASSERT(e.length == 8);
                uint64_t server_ts = ntohll(*reinterpret_cast<uint64_t*>(e.data));
                uint64_t client_ts = device::nic::CAN::RecvTimestampingXenomaiRTCANDriver::last_timestamp();
                impl.process_sync(server_ts, client_ts);
            }

            bool out_of_sync() {
                // TODO: mutex
                // TODO: get out of sync if there are no sync events anymore
                return impl.out_of_sync();
            }

            // Timestamp in us
            timefw::Time current() {
                return local_to_global(ClockDriver::current_local());
            }

            bool wait_until(const timefw::Time & global_time) {
                // Assumption: time is not too far in future (the later the less
                //             accurate will be the wakeup in case of drifting clocks)
                return ClockDriver::wait_until(global_to_local(global_time));
            }
    };

#endif // __XENOMAICLOCK_H_232A0EA7427B5B__

