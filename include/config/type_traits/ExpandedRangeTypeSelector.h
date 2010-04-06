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

#ifndef __EXPANDEDRANGETYPESELECTOR_H_FD4107871B844C__
#define __EXPANDEDRANGETYPESELECTOR_H_FD4107871B844C__


#include <stdint.h>

#include "boost/mpl/assert.hpp"


/*!
 *  \brief Type trait to select integer type with larger
 *         range than \p T.
 *
 *  For unsigned types \p T it will select an unsigned type.
 *  For signed types \p T it will select an signed type.
 *  ExpandedRangeTypeSelector<T>::type will have double
 *  bit width of T.
 */
template <typename T>
struct ExpandedRangeTypeSelector {
    BOOST_MPL_ASSERT_MSG(false, cannot_expand_range, (T));
};

template <>
struct ExpandedRangeTypeSelector<uint8_t> {
    typedef uint16_t type;
};

template <>
struct ExpandedRangeTypeSelector<uint16_t> {
    typedef uint32_t type;
};

template <>
struct ExpandedRangeTypeSelector<uint32_t> {
    typedef uint64_t type;
};

template <>
struct ExpandedRangeTypeSelector<int8_t> {
    typedef int16_t type;
};

template <>
struct ExpandedRangeTypeSelector<int16_t> {
    typedef int32_t type;
};

template <>
struct ExpandedRangeTypeSelector<int32_t> {
    typedef int64_t type;
};


#endif // __EXPANDEDRANGETYPESELECTOR_H_FD4107871B844C__

