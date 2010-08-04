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

#ifndef __AWDS_Packet_hpp__
#define __AWDS_Packet_hpp__

#include "mw/nl/awds/MAC.h"

namespace famouso {
    namespace mw {
        namespace nl {
            namespace awds {

                /** \brief A Paket for the IPC communication between Famouso and AWDS.
                 */
                struct AWDS_Packet {
                        /** \brief Constants for the paket.
                         */
                        struct constants {
                                /** \brief Constants for the paket size.
                                 */
                                struct packet_size {
                                        enum {
                                            payload = 1400
                                        /** < The maximum payload a paket can hold. */
                                        };
                                };
                                /** \brief Constants for the paket type.
                                 */
                                struct packet_type {
                                        enum {
                                            publish_fragment = 0x7B, /**< The paket contains a fragment of a published message. */
                                            publish = 0x7C,     /**< The packet contains a published message. */
                                            subscribe = 0x7D,   /**< The paket contains subjects of a node. */
                                            attributes = 0x81,  /**< The paket contains attributes of a node. */
                                            flowmgmt = 0x82     /**< The packet contains flow management information for a node. */
                                        };
                                };
                        };
                        /** \brief A header of the AWDS_Paket.
                         *
                         * The header defines the address (can be source or destination), the paket type and the size of the payload.
                         */
                        struct Header {
                                MAC addr; /**< The source or destination of the paket. */
                                uint8_t type; /**< The type of the paket. */
                            private:
                                uint8_t __pad; /**< A pad for paket alignment */
                            public:
                                uint16_t size; /**< The size of the payload in network-byte-order. */
                        }__attribute__((packed));

                        Header header; /**< The header of the paket. */
                        uint8_t data[constants::packet_size::payload]; /**< The payload of the paket. */
                }__attribute__((packed));
            }
        }
    }
}

#endif /* __AWDS_Packet_hpp__ */
