/*******************************************************************************
 *
 * Copyright (c) 2008-2010 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
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

#ifndef __canID_h__
#define __canID_h__

#include <stdint.h>

namespace famouso {
    namespace mw {
        namespace nl {
            namespace CAN {
                namespace detail {

                    /*! \brief The union defines the CAN-ID and getter/setter
                     *         methods.
                     *
                     *         In the FAMOUSO middleware the canID has a clear
                     *         structure and it is devided into three parts:
                     *         \li 8 bit priority
                     *         \li 7 bit node identifier
                     *         \li 14 bit etag that is the short network
                     *             representation of the subject of an
                     *             event.
                     *
                     *         However, the CAN-Configuration-Protocol
                     *         (famouso::mw::nl::CAN::ccp) uses the ID
                     *         in a slightly different way. For that
                     *         purpose, IDStruct contains also the
                     *         description of the CCP-parts. The union itself
                     *         provides methods for accessing these parts too.
                     *
                     *  \tparam IDStruct defines the bit-postions of the parts
                     */
                    template <typename IDStruct>
                    union __attribute__((packed)) ID {
                        uint32_t _value;
                        uint8_t _v[4];
                        typename IDStruct::parts parts;
                        typename IDStruct::parts_ccp parts_ccp;

                        /*! \brief get etag of the CAN-ID */
                        uint16_t etag() {
                            return parts._etag;
                        }

                        /*! \brief set etag of the CAN-ID */
                        void etag(uint16_t e) {
                            parts._etag = e;
                        }

                        /*! \brief get the node-ID out of CAN-ID */
                        uint8_t tx_node() {
                            return parts._tx_nodelo | (parts._tx_nodehi << 2) ;
                        }

                        /*! \brief set the node-ID on the CAN-ID */
                        void tx_node(uint8_t t) {
                            parts._tx_nodelo = t & 0x3;
                            parts._tx_nodehi = t >> 2 ;
                        }

                        /*! \brief get priority of the CAN message */
                        uint8_t prio() {
                            return parts._priolo | (parts._priohi << 3) ;
                        }

                        /*! \brief set priority of the CAN message */
                        void prio(uint8_t p) {
                            parts._priolo = p & 0x7;
                            parts._priohi = p >> 3;
                        }

                        /*! \brief determines the current CCP stage */
                        uint8_t ccp_stage() {
                            return parts_ccp._stage;
                        }

                        /*! \brief set the current CCP stage */
                        void ccp_stage(uint8_t stage) {
                            parts_ccp._stage = stage;
                        }

                        /*! \brief get the node-ID nibble for the CCP stage */
                        uint8_t ccp_nibble() {
                            return parts_ccp._nibblelo | (parts_ccp._nibblehi << 2) ;
                        }

                        /*! \brief get the node-ID nibble for the CCP stage */
                        void ccp_nibble(uint8_t nibble) {
                            parts_ccp._nibblelo = nibble;
                            parts_ccp._nibblehi = nibble >> 2;
                        }

                        /*! \brief operator provides a byte-wise access to the CAN-ID */
                        uint8_t & operator[](uint8_t i) {
                            return _v[i];
                        }
                    };

                 } /* namespace detail */

            } /* namespace CAN */
        } /* namespace nl */
    } /* namespace mw */
} /* namespace famouso */

#endif
