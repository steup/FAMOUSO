/*******************************************************************************
 *
 * Copyright (c) 2008-2009 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
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

#ifndef __Node__
#define __Node__

#include <ctime>
#include "boost/shared_ptr.hpp"
#include "debug.h"
#include "mw/nl/awds/MAC.h"

namespace famouso {
    namespace mw {
        namespace nl {
            namespace awds {

                /*! \brief A node at the AWDS network identified by his MAC address.
                 */
                class Node {
                    private:
                        /*! \brief Constructor which creates an node with a specific MAC.
                         *
                         * \param mac The MAC address of the node.
                         */
                        Node(MAC mac) :
                            _mac(mac), _time(std::time(NULL)) {
                        }

                    public:
                        /*! \brief A node at the AWDS network identified by his MAC address.
                         */
                        typedef boost::shared_ptr<Node> type;

                        /*! \brief The time in seconds when the node was last seen.
                         *
                         * \return The elapsed time in seconds.
                         */
                        int elapsed() const {
                            return static_cast<int> (std::time(NULL) - _time);
                        }

                        /*! \brief Resets the last seen time stamp.
                         */
                        void reset() {
                            _time = std::time(NULL);
                        }

                        /*! \brief Get the MAC address of the node.
                         *
                         * \return The MAC address of the node.
                         */
                        const MAC &mac() const {
                            return _mac;
                        }

                        /*! \brief The copare operator to compare if clients are same.
                         *
                         * \param o The other node to compare to.
                         * \return Returns true if the clients are the same, false otherwise.
                         */
                        bool operator==(const Node& o) const {
                            return _mac == o._mac;
                        }

                        /*! \brief prints a node to the stream.
                         *
                         *  \param out The output stream to print to.
                         */
                        void print(::logging::loggingReturnType &out) const {
                            out << _mac << " (" << ::logging::log::dec << elapsed() << ")";
                        }

                        static type create(MAC mac) {
                            type res = type(new Node(mac));
                            return res;
                        }

                    private:
                        MAC _mac;
                        clock_t _time;
                };
            } /* awds*/
        } /* nl */
    } /* mw */
} /* famouso */

namespace logging {
    /*! \brief Print a node to log.
     *
     * \param out The log to print to.
     * \param c The node to print.
     * \return The log to chain print calls.
     */
    inline ::logging::loggingReturnType &operator <<(::logging::loggingReturnType &out, const famouso::mw::nl::awds::Node::type &c) {
        c->print(out);
        return out;
    }

    /*! \brief Print a node to log.
     *
     * \param out The log to print to.
     * \param c The node to print.
     * \return The log to chain print calls.
     */
    inline ::logging::loggingReturnType &operator <<(::logging::loggingReturnType &out, const famouso::mw::nl::awds::Node &c) {
        c.print(out);
        return out;
    }
} /* logging */

#endif /* __Node__ */
