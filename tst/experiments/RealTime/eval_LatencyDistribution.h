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

#ifndef __EVAL_LATENCYDISTRIBUTION_H_EA269D010C1232__
#define __EVAL_LATENCYDISTRIBUTION_H_EA269D010C1232__

#include "eval_FileWriter.h"
#include <map>

class EvalLatencyDistribution {
        typedef std::map<int64_t, unsigned int> MapType;
        MapType distri;

        enum { intervall = 1 };
    public:

        void add_latency(int64_t lat) {
            if (intervall != 1)
                lat = (lat / intervall) * intervall;

            MapType::iterator it = distri.find(lat);
            if (it == distri.end())
                distri.insert(MapType::value_type(lat, 1));
            else
                it->second++;
        }

        void log_latency_distribution(const char * file) {
            EvalFileWriter fw(file);
            using namespace ::logging;
            MapType::iterator it = distri.begin();
            while (it != distri.end()) {
                fw.write(it->first);
                fw.write((uint64_t)it->second);
                fw.newline();
                ++it;
            }
        }
};

#endif // __EVAL_LATENCYDISTRIBUTION_H_EA269D010C1232__


