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


#ifndef __MATH_H_B17EDA6C8EF650__
#define __MATH_H_B17EDA6C8EF650__

#include "mw/afp/shared/div_round_up.h"
using famouso::mw::afp::shared::div_round_up;


inline bool odd(int a) {
    return a & 1;
}

// by stein
static inline int greatest_common_divisor(int a, int b) {
    int k, t;
    k = 0;

    while (!odd(a) && !odd(b)) {
        a >>= 1;   // a /= 2;
        b >>= 1;   // b /= 2;
        k++;
    }

    if (odd(a))
        t = -b;
    else
        t = a;

    while (t != 0) {
        while (!odd(t))
            t >>= 1;    // t /= 2;
        if (t > 0)
            a = t;
        else
            b = -t;
        t = a - b;
    }

    return a * (1 << k);
}

static inline int least_common_multiple(int a, int b) {
    return /*abs*/(a * b) / greatest_common_divisor(a,b);
}

/*!
 *  \brief  Return \p a increased to a multpile of \p b
 *  \note   Only for integral types
 */
template <typename T>
static inline T increase_to_multiple(T a, T b) {
    return div_round_up(a, b) * b;
}

/*!
 *  \brief  Return \p a reduced to a multpile of \p b
 *  \note   Only for integral types
 */
template <typename T>
static inline T reduce_to_multiple(T a, T b) {
    return (a / b) * b;
}


#endif // __MATH_H_B17EDA6C8EF650__

