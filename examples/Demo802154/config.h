/*******************************************************************************
 *
 * Copyright (c) 2008-2010
 * Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
 * Thomas Kiebel <kiebel@ivs.cs.uni-magdeburg.de>
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
//-----------------------------------------------------------------------------
//  configuration parameter
//-----------------------------------------------------------------------------
#define COMM_CHANNEL  11
#define PAN_ID        0x0001
#define SHORT_ADDR    0x0001
#define DESTINATION   0xFFFF
#define DATASIZE      16
#define MAX_PHY_PACKET_SIZE 127

//-----------------------------------------------------------------------------
// IEEE-802.15.4 Network Layer specific information
//-----------------------------------------------------------------------------
#include "maclib/phy/802154/cc2420/cc2420db/cc2420db.h"    // portmap CC2420DBK
#include "maclib/phy/802154/cc2420/cc2420interface.h"
#include "maclib/phy/802154/cc2420phy.h"
#include "maclib/dll/mac/alohamac.h"


#include "devices/nic/802154/DRV802_15_4.h"
#include "mw/nl/IEEE_802_15_4_NL.h"

//-----------------------------------------------------------------------------
// common famouso middlware includes
//-----------------------------------------------------------------------------
#include "mw/anl/AbstractNetworkLayer.h"
#include "mw/common/Event.h"
//#include "mw/common/UID.h"
#include "mw/el/EventLayer.h"
#include "mw/api/PublisherEventChannel.h"
#include "mw/api/SubscriberEventChannel.h"

#include "famouso.h"

namespace famouso {
    namespace ieee802_15_4 {
        class config {
                typedef CC2420Interface<CC2420IF, Spi<> > if_t;
                typedef wsn::phy::CC2420PHY< if_t > phy_t;
                typedef wsn::dll::mac::AlohaMAC< phy_t >      mac_t;
                typedef device::nic::ieee802_15_4::DRV802_15_4< mac_t >  driver_t;
                typedef famouso::mw::nl::IEEE_802_15_4_NL< driver_t > nl;
                typedef famouso::mw::anl::AbstractNetworkLayer< nl > anl;
            public:
                typedef famouso::mw::el::EventLayer< anl > EL;
                typedef famouso::mw::api::PublisherEventChannel<EL> PEC;
                typedef famouso::mw::api::SubscriberEventChannel<EL> SEC;
        };
    }
    typedef ieee802_15_4::config config;
}

char* subject_d = "01234567";
//-----------------------------------------------------------------------------

