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

#ifndef __EVAL_APP_DEF_H_33419184CB55EE__
#define __EVAL_APP_DEF_H_33419184CB55EE__


namespace motor1 {
    const uint64_t period = 20 * 1000;
    const uint64_t mel = 8;

    // Node 2
    const uint64_t pt_start = 100;
    const uint64_t dt_start = 200;
    const uint64_t dt_end = 10 * 1000;

    // Node 1
    const uint64_t st_start = dt_end + 100;
}

namespace sensor1 {
    const uint64_t period = 100 * 1000;
#ifndef __ETHERNET__
    const uint64_t mel = 32;
#else
    const uint64_t mel = 32000;
#endif

    // Node 1
    const uint64_t pt_start = 10 * 1000;
    const uint64_t dt_start = pt_start + 100;
    const uint64_t dt_end = dt_start + 20 * 1000;

    // Node 2
    const uint64_t st_start = 40 * 1000;
}

namespace motor2 {
    const uint64_t period = 20 * 1000;
    const uint64_t mel = 8;

/*
    // Node 4
    const uint64_t pt_start = 100;
    const uint64_t dt_start = 200;
    const uint64_t dt_end = 10 * 1000;

    // Node 3
    const uint64_t st_start = dt_end + 100;
    */
}

namespace sensor2 {
    const uint64_t period = 10 * 1000;
#ifndef __ETHERNET__
    const uint64_t mel = 4;
#else
    const uint64_t mel = 16000;
#endif

/*
    // Node 3
    const uint64_t pt_start = 0;
    const uint64_t dt_start = pt_start + 100;
    const uint64_t dt_end = 0;

    // Node 4
    const uint64_t st_start = dt_end + 100;
    */
}

namespace emrgstop {
    const uint64_t period = 50 * 1000;
    const uint64_t mel = 0;

/*
    // Node 3
    const uint64_t pt_start = 0;
    const uint64_t dt_start = pt_start + 100;
    const uint64_t dt_end = 0;

    // Node 4
    const uint64_t st_start = dt_end + 100;
    */
}


#endif // __EVAL_APP_DEF_H_33419184CB55EE__

