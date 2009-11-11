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
 *    $Id$
 *
 ******************************************************************************/
#ifndef __Uninitialized_h__
#define __Uninitialized_h__

#include <stdint.h>

namespace object {

    /*! \brief Uninitialized presents a container of an object of type T
     *
     *         An object of type Uninitialized contains at least memory
     *         for an object of type T. Thus, it is possible to put an
     *         object of type T in the container or construct it with the
     *         aid of the placement new operator directly to the provided
     *         storage within the container.
     *
     * \tparam T is the element type that is stored.
     */
    template<typename T>
    struct Uninitialized {
        //! the provided storage for an object of type T
        uint8_t content[sizeof(T)];

        //! operator is provided for some std::algorithms
        bool operator == (const Uninitialized& t) {
            return !!(this == &t);
        }

        /*! \brief placement new operator to allow a constructor call
         *         on pre-allocated memory
         */
        void *operator new(size_t size, void* buffer) {
            return buffer;
        }
    };

} /* namespace object */

#endif

