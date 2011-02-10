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

#ifndef __SLOTSCHEDULER_H_A4ECF9C5163E41__
#define __SLOTSCHEDULER_H_A4ECF9C5163E41__

#include <algorithm>
#include "../math.h"

#include "mw/afp/defrag/detail/BitArray.h"

namespace famouso {
    namespace mw {
        namespace rt_net_sched {

            typedef famouso::mw::afp::defrag::detail::Bit Bit;
            typedef famouso::mw::afp::defrag::detail::BitArray<0> BitArray;


            class SlotScheduler {

                    BitArray free_slots;

                public:
                    SlotScheduler() :
                        free_slots(true)
                    {
                        free_slots.resize(1);
                    }

                    unsigned int cycle_length() {
                        return free_slots.size();
                    }


                    // hinzufügen: Netz-ID, ...
                    // Interface-Funktion

                    // protected
                    // split: feasablility check, resize, reserve?
                    // Parameter in atomic slots, shift_min and shift_width may be -1 to be ignored
                    bool reserve(unsigned int period, unsigned int length, int shift_min, int shift_width, unsigned int & shift) {

                        enum { invalid = (unsigned int)(-1) };

                        unsigned int old_cycle_len = cycle_length();
                        unsigned int new_cycle_len = least_common_multiple(old_cycle_len, period);

                        // First fitting slot search
                        unsigned int ffs_begin;
                        unsigned int ffs_end;

                        // Define search window
                        if (shift_min >= 0) {
                            ffs_begin = shift_min;
                            if (shift_width >= 0)
                                ffs_end = shift_min + std::min(std::min(period, old_cycle_len), (unsigned int)shift_width);
                            else
                                ffs_end = shift_min + std::min(period, old_cycle_len);
                        } else {
                            ffs_begin = 0;
                            ffs_end = std::min(period, old_cycle_len);
                        }

                        // Check for schedulability
                        unsigned int v = ffs_begin, vr, vr_end, vl, vl_end;
                        while (v < ffs_end) {
                            vr = v;
                            vr_end = v + new_cycle_len;
                            do {
                                vl = vr;
                                vl_end = vr + length;
                                do {
                                    if (free_slots[vl % old_cycle_len].get() == false) {
                                        // Slot to short
                                        v += vl - vr;
                                        goto find_next_ffs;
                                    }
                                    ++vl;
                                } while (vl < vl_end);
                                vr += period;
                            } while (vr < vr_end);

                            // No goto find_next_ffs -> Schedulable
                            break;

                        find_next_ffs:
                            ++v;
                        }

                        // Network communication channel cannot be established (not schedulable)
                        if (v >= ffs_end)
                            return false;

                        unsigned int ffs_start = v;

                        // Increase free_slots BitArray size to new cycle length
                        if (old_cycle_len != new_cycle_len) {
                            // TODO: lot of potential for optimization
                            free_slots.resize(new_cycle_len);
                            for (unsigned int i = 0; i < old_cycle_len; ++i) {
                                bool b = free_slots[i].get();
                                for (unsigned int j = i + old_cycle_len; j < new_cycle_len; j += old_cycle_len)
                                    free_slots[j] = b;
                            }
                        }

                        // Clear free bits for reserved medium time
                        for (vr = ffs_start; vr < new_cycle_len; vr += period) {
                            for (vl = vr; vl < vr + length; ++vl) {
                                free_slots[vl % new_cycle_len].clear();
                            }
                        }

                        shift = ffs_start;

                        return true;
                    }

                    void free(unsigned int period, unsigned int length, unsigned int shift) {
                        // TODO: reduce cycle length
                        unsigned int old_cycle_len = free_slots.size();

                        // Clear free bits for reserved medium time
                        unsigned int j, k;
                        for (j = shift; j < old_cycle_len; j += period) {
                            for (k = j; k < j + length; ++k) {
                                free_slots[k % old_cycle_len].set();
                            }
                        }
                    }

                    void log_free_list() {
                        ::logging::log::emit() << "Free slots (cycle_length="
                                               << ::logging::log::dec << free_slots.size()
                                               << ", x=free):\n\t|";
                        for (unsigned int i = 0; i < free_slots.size(); ++i) {
                            char c = free_slots[i].get() ? 'x' : '_';
                            ::logging::log::emit() << c;
                        }
                        ::logging::log::emit() << "|\n";
                        ::logging::log::emit() << "\t ";
                        for (unsigned int i = 0; i < free_slots.size(); ++i) {
                            char c = '0' + (i % 10);
                            ::logging::log::emit() << c;
                        }
                        ::logging::log::emit() << "\n";
                        ::logging::log::emit() << "\t ";
                        ::logging::log::emit() << "0         ";
                        for (unsigned int i = 10; i < free_slots.size()+9; i += 10) {
                            char numbuf[10];
                            uint8_t numlen = 0;
                            unsigned int num = i;
                            while (num) {
                                numbuf[numlen] = '0' + (num % 10);
                                numlen++;
                                num /= 10;
                            }
                            for (int j = numlen - 1; j >= 0; j--)
                                ::logging::log::emit() << numbuf[j];
                            for (int j = 10; j > numlen; j--)
                                ::logging::log::emit() << ' ';
                        }
                        ::logging::log::emit() << "\n";
                    }
            };

        } // namespace rt_net_sched
    } // namespace mw
} // namespace famouso

#endif // __SLOTSCHEDULER_H_A4ECF9C5163E41__

