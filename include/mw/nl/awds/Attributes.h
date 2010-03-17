/*******************************************************************************
 *
 * Copyright (c) 2008-2010 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
 *               2009-2010 Michael Kriese <kriese@cs.uni-magdeburg.de>
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

#ifndef _Attributes_h_
#define _Attributes_h_

#include <cstring>
#include "boost/shared_ptr.hpp"
#include "boost/noncopyable.hpp"
#include "mw/nl/awds/AWDS_Packet.h"
#include "mw/attributes/TTL.h"
#include "mw/attributes/filter/find.h"

namespace famouso {
    namespace mw {
        namespace nl {
            namespace awds {
                namespace detail {

                    /*! \brief try to find the Attribute A in a data pointer
                     *
                     *  \tparam A is the type of the attribute
                     *
                     *  \returns a pointer to the location of attribute A
                     *           within the event, and NULL else
                     *
                     *  \todo this function has to be adapted if the binary
                     *        representation of attributes within event
                     *        change
                     *
                     *  \todo at the moment, only the first attribute is
                     *        checked
                     */
                    template< typename A, typename B, typename C >
                    static inline A* find(B *data, C size) {
                        for (C p = 0; p < size; p++) {
                            if (data[p] == A::base_type::id)
                                return reinterpret_cast<A*> (&data[p]);
                        }
                        return reinterpret_cast<A*> (0);
                    }

                } // namespace detail

                class Attributes: boost::noncopyable {
                    private:
                        typedef attributes::TTL<0> TTL;

                    public:
                        typedef boost::shared_ptr<Attributes> type;


                        void set(awds::AWDS_Packet &p) {
                            // if this is not an attributes packet
                            if (p.header.type != AWDS_Packet::constants::packet_type::attributes)
                                return;

                            TTL * t = detail::find<TTL>(p.data, ntohs(p.header.size));
                            if (t)
                                ttl.set(t->get());
                        }

                        void get(awds::AWDS_Packet &p) {
                        }

                        /*! \brief Checks weather attributes are matching
                         *
                         *  \param o The other attributes.
                         *  \return true if all attributes of this are less than o, otherwise false.
                         */
                        bool operator<(const Attributes & o) const {
                            if (ttl.get() >= o.ttl.get())
                                return false;

                            return true;
                        }

                        /*! \brief Checks weather attributes are matching
                         *
                         *  \param o The other attributes.
                         *  \return true if all attributes of this are more than o, otherwise false.
                         */
                        bool operator>(const Attributes & o) const {
                            if (ttl.get() <= o.ttl.get())
                                return false;

                            return true;
                        }

                        /*! \brief Checks weather attributes are matching
                         *
                         *  \param o The other attributes.
                         *  \return true if all attributes of o are equal to this, otherwise false.
                         */
                        bool operator==(const Attributes & o) const {
                            if (ttl.get() != o.ttl.get())
                                return false;

                            return true;
                        }

                        /*! \brief Checks weather attributes are matching
                         *
                         *  \param o The other attributes.
                         *  \return true if all attributes of o are more than or equal to this, otherwise false.
                         */
                        bool operator<=(const Attributes & o) const {
                            return !(*this > o);
                        }

                        /*! \brief Checks weather attributes are matching
                         *
                         *  \param o The other attributes.
                         *  \return true if all attributes of o are less than or equal to this, otherwise false.
                         */
                        bool operator>=(const Attributes & o) const {
                            return !(*this < o);
                        }

                        /*! \brief Checks weather attributes are matching
                         *
                         *  \param o The other attributes.
                         *  \return true if one attributes of o are not equal to this, otherwise false.
                         */
                        bool operator!=(const Attributes &o) const {
                            return !(*this == o);
                        }

                        /*! \brief Creates an empty attributes instance.
                         *
                         *  \return An instance of attributes.
                         */
                        static type create() {
                            type res = type(new Attributes());
                            return res;
                        }

                    private:
                        TTL ttl;
                };

                /*! \brief Checks weather attributes are matching
                 *
                 *  \param a The first attributes.
                 *  \param b The second attributes.
                 *  \return true if all attributes of a are less than or equal to b, otherwise false.
                 */
                inline bool operator<=(const Attributes::type &a, const Attributes::type &b) {
                    return *a <= *b;
                }

                /*! \brief Checks weather attributes are matching
                 *
                 *  \param a The first attributes.
                 *  \param b The second attributes.
                 *  \return true if all attributes of a are more than or equal to b, otherwise false.
                 */
                inline bool operator>=(const Attributes::type &a, const Attributes::type &b) {
                    return *a >= *b;
                }

                /*! \brief Checks weather attributes are matching
                 *
                 *  \param a The first attributes.
                 *  \param b The second attributes.
                 *  \return true if all attributes of a are less than b, otherwise false.
                 */
                inline bool operator<(const Attributes::type &a, const Attributes::type &b) {
                    return *a < *b;
                }

                /*! \brief Checks weather attributes are matching
                 *
                 *  \param a The first attributes.
                 *  \param b The second attributes.
                 *  \return true if all attributes of a are more than b, otherwise false.
                 */
                inline bool operator>(const Attributes::type &a, const Attributes::type &b) {
                    return *a > *b;
                }

            } /* awds */
        } /* nl */
    } /* mw */
} /* famouso */
#endif /* _Attributes_h_ */
