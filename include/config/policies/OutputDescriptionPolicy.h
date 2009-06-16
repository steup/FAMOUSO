/*******************************************************************************
 *
 * Copyright (c) 2008, 2009 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
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

#ifndef __OutputDescriptionPolicy_h__
#define __OutputDescriptionPolicy_h__

#include <iostream>

/*! \brief OutputDescriptionPolicy prints the description of
 *         the configuration if such a description exists.
 */
template <typename R=void>
struct OutputDescriptionPolicy {
    /*! \brief The method is provided for calling within the context of a
     *         selector template. As the name of the policys class describe this
     *         is intended to be called for a subtype with containing the description.
     *
     *  \tparam R is the return value of the static process method and its
     *          default is \c void
     *  \tparam T is a type normally a configuration type/class
     *  \tparam SubType is the type and you can call the desc method
     *          that gives you a const char* const pointer to the
     *          description. If the policy is used in the context of
     *          IF_CONTAINS_TYPE_(NAME) it is guaranteed, that SubType
     *          is the requested type.
     */
    template< typename T, typename SubType>
    static __attribute__((always_inline)) R process() {
        std::cout << SubType::desc() << std::endl;
        return R();
    }
};

#endif

