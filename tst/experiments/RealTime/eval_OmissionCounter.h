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

#ifndef __EVAL_OMISSIONCOUNTER_H_9DB12B2503BF78__
#define __EVAL_OMISSIONCOUNTER_H_9DB12B2503BF78__

class EvalOmissionCounter {
        bool received_anything;
        uint64_t omission_counter;
        uint64_t consecutive_omissions;
        uint64_t events;
    public:
        EvalOmissionCounter() :
            received_anything(false),
            omission_counter(0),
            consecutive_omissions(0),
            events(0) {
        }

        void received_event() {
            if (!received_anything) {
                // Filter out omissions before first received event
                received_anything = true;
                consecutive_omissions = 0;
            } else {
                // Filter out omissions after last received event
                omission_counter += consecutive_omissions;
                consecutive_omissions = 0;
            }
            events++;
        }

        void omitted_event() {
            consecutive_omissions++;
        }

        void log_results(const famouso::mw::Subject & s) {
            ::logging::log::emit()
                << "Subscriber on channel " << s << ":\n"
                << "\t=> omission count (in between of received events): " << omission_counter
                << "\n\t=> received events: " << events << "\n";
        }
};

#endif // __EVAL_OMISSIONCOUNTER_H_9DB12B2503BF78__


