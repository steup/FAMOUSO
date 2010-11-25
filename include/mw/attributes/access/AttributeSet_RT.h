/*******************************************************************************
 *
 * Copyright (c) 2008-2010 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
 *                    2010 Marcus Foerster <MarcusFoerster1@gmx.de>
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

#ifndef _AttributeSet_RT_h_
#define _AttributeSet_RT_h_

#include <stdint.h>

#include "mw/attributes/access/AttributeSetHeader_RT.h"

namespace famouso {
    namespace mw {
        namespace attributes {
            namespace access {

                class AttributeSet_RT : public AttributeSetHeader_RT {
                    protected:
                        AttributeSet_RT() {
                            // Visibility
                        }

                    public:
                        /*!
                         * \brief Searches for the attribute given as a template argument in the
                         *  binary representation of this attribute set and returns it
                         *
                         * The target attribute is searched for using the high density property and
                         *  the ID of the given attribute type. The returned instance can then
                         *  be used to access the value.
                         * If the given attribute could not be found in the set, NULL is returned.
                         *
                         * \tparam Attr The attribute type which should be searched for
                         *
                         * \return An instance of Attr or NULL if the attribute could not be found
                         */
                        template <typename Attr>
                        Attr* find_rt() {
                            uint8_t* const data = reinterpret_cast<uint8_t* const>(this);

                            return (famouso::mw::attributes::detail::find<Attr>(&data[0]));
                        }

                        /*!
                         * \brief Searches for the attribute given as a template argument in the
                         *  binary representation of this attribute set and returns it.
                         *
                         * The target attribute is searched for using the high density property and
                         *  the ID of the given attribute type. The returned instance can then
                         *  be used to access the value.
                         * If the given attribute could not be found in the set, NULL is returned.
                         *
                         * \tparam Attr The attribute type which should be searched for
                         *
                         * \return A constant instance of Attr or NULL if the attribute could
                         *  not be found
                         */
                        template <typename Attr>
                        const Attr* find_rt() const {
                            uint8_t* const data = reinterpret_cast<uint8_t* const>(this);

                            return (famouso::mw::attributes::detail::find<Attr>(&data[0]));
                        }

                        /**
                         * \brief Returns the number of bytes used for the complete encoded
                         *  attribute set.
                         *
                         * This also includes the number of bytes used for the set header itself.
                         *
                         * \return The number of bytes used for this attribute set
                         */
                        uint16_t length() const {
                            return (contentLength() + headerLength());
                        }

                        /**
                         * \brief Returns the number of attributes encoded in this set.
                         *
                         * \return The number of encoded attributes
                         */
                        uint16_t size() const {
                            const uint8_t* ptr = reinterpret_cast<const uint8_t*>(this);

                            uint16_t result = 0;

                            // TODO: This is a copy-paste-version of the find method, consider implementing
                            //  an iterator class

                            ptr += headerLength();

                            // The pointer were the given sequence ends
                            const uint8_t* const targetPtr = ptr + contentLength();

                            typedef famouso::mw::attributes::access::Attribute_RT attrType;

                            while (ptr < targetPtr) {
                                ++result;

                                // We let the attribute class determine its overall size to skip it
                                ptr += reinterpret_cast<const attrType* const>(&ptr[0])->length();
                            }

                            // We iterated the complete set and so return the counted attributes
                            return (result);
                        }

                        /**
                         * \brief Copies the content of this set (i.e. the encoded attributes) to the
                         *  specified buffer
                         */
                        void content(uint8_t* buffer) const {
                            const uint8_t* const data = reinterpret_cast<const uint8_t* const>(this);

                            memcpy(buffer, &data[headerLength()], contentLength());
                        }

                        // TODO: Implement methods for manipulation of the attribute set at runtime
                };

            } // end namespace access
        } // end namespace attributes
    } // end namespace mw
} // end namespace famouso

#endif // _Attribute_h_
