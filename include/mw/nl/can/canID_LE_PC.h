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

#ifndef __canID_LE_PC_h__
#define __canID_LE_PC_h__

#include <stdint.h>
#include "mw/nl/can/canID.h"

namespace famouso {
    namespace mw {
        namespace nl {
            namespace CAN {
                namespace detail {

                    /*! \brief CAN-ID parts description for little endian machines
                     */
                    struct __attribute__((packed)) famouso_CAN_ID_LE_PC {
                        typedef class __attribute__((packed)) {
                            public:
                            uint16_t _etag      : 14;
                            uint8_t  _tx_nodelo :  2;
                            uint8_t  _tx_nodehi :  5;
                            uint8_t  _fragment  :  1;
                            uint8_t  _priolo    :  2;
                            uint8_t  _priohi    :  5;
                        } parts;
                        typedef class __attribute__((packed)) {
                            public:
                            uint16_t _etag      : 14;
                            uint8_t  _nibblelo  :  2;
                            uint8_t  _nibblehi  :  2;
                            uint8_t  _stage     :  4;
                        } parts_ccp;
                    };

                 } /* namespace detail */

            } /* namespace CAN */
        } /* namespace nl */
    } /* namespace mw */
} /* namespace famouso */

#endif
