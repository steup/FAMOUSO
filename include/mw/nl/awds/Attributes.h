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
#include "logging.h"

#ifdef RANDOM_ATTRIBUTES
#include <cstdlib>
#endif

namespace famouso {
    namespace mw {
        namespace nl {
            namespace awds {
                /** \brief This namepsace contains helper functions and classes for the attributes.
                 *
                 * \todo This namepsace is temporary and should be removed later.
                 */
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
                     *        change                     *
                     */
                    template< typename A, typename B >
                    static inline A* find(B *data) {
                        for (uint16_t p = 0; p < AWDS_Packet::constants::packet_size::payload; p++) {
                            if (data[p] == A::base_type::id)
                                return reinterpret_cast<A*> (&data[p]);
                        }
                        return reinterpret_cast<A*> (0);
                    }

                    /*!\brief   defines a configurable attribute
                     *
                     * \tparam  v describes the initial value to be set
                     */
                    template< uint8_t t, uint8_t v >
                    class Attribute: public attributes::EmptyAttribute {
                        public:
                            typedef Attribute<t, 0> base_type;
                            typedef attributes::tags::integral_const_tag compare_tag;
                            typedef Attribute type;
                            enum {
                                size = 2,
                                id = t
                            };
                            static uint8_t value() {
                                return v;
                            }

                            Attribute() {
                                data[0] = id;
                                data[1] = value();
                            }

                            uint8_t get() const {
                                return data[1];
                            }
                            void set(uint8_t tmp) {
                                data[1] = tmp;
                            }
                        private:
                            uint8_t data[size];
                    };

                } // namespace detail

                /* \brief A class for holding attributes of publisher, subscriber or network.
                 *
                 * \todo This class will be deleted and replaced by attributes framewok in future.
                 */
                class Attributes: boost::noncopyable {
                    private:
                        typedef attributes::TTL<0> TTL; /**< The time-to-live or hop-count to the node. */
                        typedef detail::Attribute<'L', 0> Latency; /**< The latency or half round trip time to the node. */
                        typedef detail::Attribute<'B', 0> Bandwidth; /**< The bandwith to the node in bytes per second. */
                        typedef detail::Attribute<'P', 0> PaketLoss; /**< The packet loss rate to the node in percent. */

                        /* \brief Copy the data and search for attributes
                         *
                         * \param d the data pointer
                         * \param s the size of the data
                         *
                         * \todo Copy only attributes, and not the whole data.
                         */
                        void set(uint8_t *d, uint16_t s) {

                            std::memcpy(data, d, s);

                            ttl = detail::find<TTL>(data);
                            lat = detail::find<Latency>(data);
                            band = detail::find<Bandwidth>(data);
                            ploss = detail::find<PaketLoss>(data);
                        }

                    public:
                        /** \copydoc Attributes */
                        typedef boost::shared_ptr<Attributes> type;

                        /**
                         * \brief Copy the attributes from the packet to this attribute instance.
                         *
                         * \param p The packet to copy from.
                         */
                        void set(awds::AWDS_Packet &p) {
                            // if this is not an attributes packet
                            if (p.header.type != AWDS_Packet::constants::packet_type::attributes)
                                return;

                            set(p.data, ntohs(p.header.size));
                        }

                        /* \brief Checks weather the given attributes are matching.
                         *
                         * \param a The attribute wich has to be less or equal to the b.
                         * \param b The attribute wich has to be more or equal to a or null.
                         * \return Returns true if b is null or a is less or equalt to b, otherwise false.
                         */
                        template< typename Attrib >
                        bool match(Attrib *a, Attrib *b) const {
                            // if attrib b is empty check is always true
                            if (!b)
                                return true;

                            // if attrib b is not empty, attrib a has to be not empty too, so check if it matches
                            if (a && a->get() <= b->get())
                                return true;

                            // attributes doesn't match
                            return false;
                        }

                        /*! \brief Checks weather all attributes are matching
                         *
                         *  if an attribute in o is null, the check for this attribute will always match.
                         *
                         *  \param o The other attributes.
                         *  \return true if all attributes of this are less than or equal to o, otherwise false.
                         */
                        bool operator<=(const Attributes & o) const {
                            // check if ttl is matched
                            if (!match(ttl, o.ttl))
                                return false;

                            // check if latency is matched
                            if (!match(lat, o.lat))
                                return false;

                            // check if latency is matched
                            if (!match(band, o.band))
                                return false;

                            // check if latency is matched
                            if (!match(ploss, o.ploss))
                                return false;

                            return true;
                        }

                        /*! \brief print the attributes to the given stream.
                         *
                         *  \param out the output stream to print to.
                         */
                        void print(::logging::loggingReturnType &out) const {
                            out << ::logging::log::dec;
                            if (ttl)
                                out << " TTL: " << (int) ttl->get();
                            if (lat)
                                out << " Latency: " << (int) lat->get();
                            if (band)
                                out << " Bandwidth: " << (int) band->get();
                            if (ploss)
                                out << " PaketLoss: " << (int) ploss->get();
                        }

                        /*! \brief Creates an empty attributes instance.
                         *
                         *  \return An instance of attributes.
                         */
                        static type create() {
                            type res = type(new Attributes());
                            return res;
                        }

                        /*! \brief Creates an empty attributes instance.
                         *
                         *  \param p An AWDS_Packet to load attributes from.
                         *  \return An instance of attributes.
                         */
                        static type create(AWDS_Packet &p) {
                            type res = type(new Attributes());
                            res->set(p);
                            return res;
                        }

#ifdef RANDOM_ATTRIBUTES
                        /*! \brief Creates a random attributes instance.
                         *
                         *  \return An instance of attributes.
                         */
                        static type createRand() {
                            type res = type(new Attributes());
                            uint8_t data[8] = { 'T', (rand() % 10 + 1), 'L', (rand() % 10 + 1), 'P', (rand() % 10 + 1), 'B', (rand() % 10
                                            + 1) };
                            res->set(data, 8);
                            return res;
                        }
#endif

                    private:
                        uint8_t data[AWDS_Packet::constants::packet_size::payload]; /**< The data where the attributes are stored. */
                        TTL *ttl; /**< A pointer to the TTL attribute. */
                        Latency *lat; /**< A pointer to the latency attribute. */
                        Bandwidth *band; /**< A pointer to the bandwidth attribute. */
                        PaketLoss *ploss; /**< A pointer to the packet loss rate attribute. */
                };

                /*! \brief Checks weather attributes are matching
                 *
                 *  Attributes::type is a typedef of boost::shared_ptr<Attributes>.
                 *  \param a The first attributes.
                 *  \param b The second attributes.
                 *  \return true if all attributes of a are less than or equal to b, otherwise false.
                 */
                inline bool operator<=(const Attributes::type &a, const Attributes::type &b) {
                    // so we have to dereference and compare
                    return *a <= *b;
                }

            } /* awds */
        } /* nl */
    } /* mw */
} /* famouso */

namespace logging {

    /*! \brief Print the attributes to the given stream.
     *
     *  Attributes::type is a typedef of boost::shared_ptr<Attributes>.
     *
     *  \param out the output stream to print to.
     *  \param att the attributes to print.
     *  \return the output stream for chaining.
     */
    inline ::logging::loggingReturnType &operator<<(loggingReturnType &out, const famouso::mw::nl::awds::Attributes::type &att) {
        att->print(out);
        return out;
    }

} // namespace logging
#endif /* _Attributes_h_ */
