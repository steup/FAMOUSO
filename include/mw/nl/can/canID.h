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

#ifndef __canID_h__
#define __canID_h__

namespace famouso {
    namespace mw {
        namespace nl {
            namespace CAN {
                namespace detail {

                    template <typename IDStruct>
                    union __attribute__((packed)) ID {
                        uint32_t _value;
                        uint8_t _v[4];
                        typename IDStruct::parts parts;
                        typename IDStruct::parts_ccp parts_ccp;

                        uint16_t etag() {
                            return parts._etag;
                        }

                        void etag(uint16_t e) {
                            parts._etag = e;
                        }

                        uint8_t tx_node() {
                            return parts._tx_nodelo | (parts._tx_nodehi << 2) ;
                        }

                        void tx_node(uint8_t t) {
                            parts._tx_nodelo = t & 0x3;
                            parts._tx_nodehi = t >> 2 ;
                        }

                        uint8_t prio() {
                            return parts._priolo | (parts._priohi << 3) ;
                        }

                        void prio(uint8_t p) {
                            parts._priolo = p & 0x7;
                            parts._priohi = p >> 3;
                        }

                        uint8_t ccp_stage() {
                            return parts_ccp._stage;
                        }

                        void ccp_stage(uint8_t stage) {
                            parts_ccp._stage = stage;
                        }

                        uint8_t ccp_nibble() {
                            return parts_ccp._nibblelo | (parts_ccp._nibblehi << 2) ;
                        }

                        void ccp_nibble(uint8_t nibble) {
                            parts_ccp._nibblelo = nibble;
                            parts_ccp._nibblehi = nibble >> 2;
                        }

                        uint8_t & operator[](uint8_t i) {
                            return _v[i];
                        }
                    };

                } /* namespace detail */

                namespace CANARY {
                    struct __attribute__((packed)) famouso_CAN_ID_LE_CANARY {
                        typedef class __attribute__((packed)) {
                            uint8_t  _pad       :  3;
                            public:
                            uint16_t _etag      : 14;
                            uint8_t  _tx_nodelo :  2;
                            uint8_t  _tx_nodehi :  5;
                            int8_t   _priolo    :  3;
                            int8_t   _priohi    :  5;
                        } parts;
                        typedef class __attribute__((packed)) {
                            uint8_t  _pad       :  3;
                            public:
                            uint16_t _etag      : 14;
                            uint8_t  _nibblelo  :  2;
                            uint8_t  _nibblehi  :  2;
                            uint8_t  _stage     :  4;
                        } parts_ccp;
                    };
                    typedef famouso::mw::nl::CAN::detail::ID<famouso_CAN_ID_LE_CANARY> ID;
                }

                namespace PEAK {
                    // little endian defined ID for CAN on PCs
                    struct __attribute__((packed)) famouso_CAN_ID_LE_PC {
                        typedef class __attribute__((packed)) {
                            public:
                            uint16_t _etag      : 14;
                            uint8_t  _tx_nodelo :  2;
                            uint8_t  _tx_nodehi :  5;
                            int8_t   _priolo    :  3;
                            int8_t   _priohi    :  5;
                        } parts;
                        typedef class __attribute__((packed)) {
                            public:
                            uint16_t _etag      : 14;
                            uint8_t  _nibblelo  :  2;
                            uint8_t  _nibblehi  :  2;
                            uint8_t  _stage     :  4;
                        } parts_ccp;
                    };
                    typedef famouso::mw::nl::CAN::detail::ID<famouso_CAN_ID_LE_PC> ID;
                }

            } /* namespace CAN */
        } /* namespace nl */
    } /* namespace mw */
} /* namespace famouso */

#endif
