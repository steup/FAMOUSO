/*******************************************************************************
 *
 * Copyright (c) 2011 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
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
 * $Id: $
 *
 ******************************************************************************/

#ifndef __EVENTLAYERMISSINGPOLICY_H_298C5A34AFA7F2__
#define __EVENTLAYERMISSINGPOLICY_H_298C5A34AFA7F2__

#include "assert/staticerror.h"

struct EventLayerMissingPolicy {
    /*! \brief The method is provided for calling within the context of a
     *         selector template. As the name of the policy class describes this
     *         is intended to be called if the EventLayer is configured within
     *         the declarative configuration description.
     *
     *  \tparam T is a type normally a configuration type/class
     *  \tparam SubType is a type and you can instantiate an
     *          object of it. If the policy is used in the context of
     *          IF_CONTAINS_TYPE_(NAME) it is guaranteed, that SubType is the
     *          requested type.
     *  \tparam R is the return value of the static process method and its
     *          default is \c void
     *
     */
    template< typename T, typename SubType, typename R>
    static  R process() {
        FAMOUSO_STATIC_ASSERT_ERROR(
            sizeof(T) == 0,
            declarative_configuration_does_not_support_an_EL_type,
            ());

        return R();
    }
};



#endif // __EVENTLAYERMISSINGPOLICY_H_298C5A34AFA7F2__

