/*******************************************************************************
*
* Copyright (c) 2010 Marcus Foerster <MarcusFoerster1@gmx.de>
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
* $Id:$
*
******************************************************************************/

#ifndef __SMALLESTSIGNEDTYPESELECTOR_H_40A78CED678505__
#define __SMALLESTSIGNEDTYPESELECTOR_H_40A78CED678505__

#include <stdint.h>

#include "config/type_traits/if_select_type.h"

/*!
 *  \brief Type trait to select smallest signed integer type
 *         that can store \p value.
 */
template <int64_t value>
struct SmallestSignedTypeSelector {
        typedef typename if_select_type<
                          (static_cast<int8_t>(value) == value),
                          int8_t,
                          typename if_select_type<
                           (static_cast<int16_t>(value) == value),
                           int16_t,
                           typename if_select_type<
                            (static_cast<int32_t>(value) == value),
                            int32_t,
                            int64_t
                           >::type
                          >::type
                         >::type type;
};


#endif // __SMALLESTSIGNEDTYPESELECTOR_H_40A78CED678505__
