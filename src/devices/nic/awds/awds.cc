/*
 * AWDS, a wrapper to communicate with the awds from EUK
 * Copyright (C) 2006 Dipl. Inform. Michael Schulze, Dipl. Inform Andre' Herms
 * Embedded Systems and Operating Systems
 * Department of Distributed Systems (IVS)
 * Faculty of Computer Science
 * Otto-von-Guericke-University Magdeburg, Germany
 *
 * mschulze@ivs.cs.uni-magdeburg.de
 * aherms@ivs.cs.uni-magdeburg.de
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA.
 */

/*$Id: awds.cc 204 2008-02-14 12:38:29Z lindhors $*/

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#include <gea/Blocker.h>
#include <gea/ObjRepository.h>
#include <gea/API.h>
#include <awds/routing.h>
#include <awds/Flood.h>
#include <awds/UnicastPacket.h>

#include <awds/toArray.h>


#include "anl/md/md.h"
#include "cosmic/cosmiccfg.h"
#include "nic/awds.h"

#include <vector>

#define AWDS_BC_PUBLISH_TYPE 0x7C // type for broadcast packets
#define AWDS_UC_PUBLISH_TYPE 0x7C // type for unicast packets

#define AWDS_BC_SUBSCRIBE_TYPE 0x7D

using namespace std;


struct AwdsSubscriber {

    uint64_t       subject;
    awds::NodeId   id;
    gea::AbsTime   expire;
    int distance;
};


struct awds_nic_dev {

    gea::Blocker   blocker; ///< used for periodic subscription announcement.
    awds::Routing *routing;

    priority_desc_t prio_desc;
    priority_t      priority;

    /** \brief this variable is set when there is at least one subscriber
     *         and a receive callback is registered.
     *
     */
    bool listening;

    /** flag to indicate if announce subscriptions
     */
    bool subscribing;


    /** \brief flag to indicate that we are publishing an
     *  listen for subscription announcements
     */
    bool bound_pub;


    /** \brief TTL (maximum hop count) used for subscription
     */
    int subTTL;

    /** maximum number of receivers to use unicast instead
     *  of flooding
     */
    int max_uc_receivers;

    gea::Duration subscr_period; ///< period of subscription announcements

    vector<struct AwdsSubscriber> subscriberSet;
    vector<uint64_t> subscriptions;

    void init() {

	listening   = false;
	subscribing = false;
	bound_pub   = false;

	prio_desc.level = PRIORITY_CLASS_NRT;
	prio_desc.start = 1;
	prio_desc.range = 1;
	priority = 1;

	subTTL = 10;
	max_uc_receivers = 1;

	subscriptions.clear();
	subscr_period = 1.9;
    }
};

/** \brief single global object for the awds device.
 *   What a @#$%! There should be a parameter, not a global object.
 */
static struct awds_nic_dev awds_dev_1;

/** common receive function for unicast and broadcast messages
 */
static void awds_receive_common(char *ptr, unsigned size) {

    subject_t       subject;
    st_message_t    st_msg;


    subject.val = fromArray<uint64_t>(ptr);

    st_msg.data.len  = size  - 8;
    st_msg.subject   = &subject;
    st_msg.data.val  = (uint8_t *)(ptr + 8);
    st_msg.priority  = 1;
    st_msg.priority_desc = &(awds_dev_1.prio_desc);

    std::ostream& os = GEA.dbg() << "received packet, subject:"
				 << hex << subject.val << dec << " data:";

    for (int i = 0; i < st_msg.data.len;  ++i) {
	char prev_fill = os.fill('0');
	os.width(2);
	os << hex << (int)(st_msg.data.val[i]) << dec << " ";
	os.fill(prev_fill);
    }
    os << std::endl;

    md_handle_msg_recv(&st_msg);

}

/** \brief data receive function for broadcast traffic */
void awds_receive_bc(awds::BasePacket *p, void *data) {

    assert(p->getType() == awds::PacketTypeFlood);

    awds_receive_common(p->buffer + awds::Flood::FloodHeaderEnd,
			p->size   - awds::Flood::FloodHeaderEnd );

    return;
}

/** \brief data receive function for unicast traffic */
void awds_receive_uc(awds::BasePacket *p, void *data) {

    assert(p->getType() == awds::PacketTypeUnicast);

    awds_receive_common(p->buffer + awds::UnicastPacket::UnicastPacketEnd,
			p->size - awds::UnicastPacket::UnicastPacketEnd);

}


void awds_receive_subscription(awds::BasePacket *p, void *data) {

    struct awds_nic_dev *dev = static_cast<struct awds_nic_dev *>(data);

    awds::Flood flood(*p);

    const char *ptr = p->buffer + awds::Flood::FloodHeaderEnd;

    uint32_t initialTTL = fromArray<uint32_t>(ptr);
    ptr += 4;
    uint16_t num = fromArray<uint16_t>(ptr);
    ptr += 2;

    /* iterate over all subjects */
    for ( uint16_t i = 0; i < num; ++i) {
	uint64_t subj = fromArray<uint64_t>(ptr);
	ptr += 8;

	/* try to find an old entry */
	int pos = -1;
	for (int j = 0; j < dev->subscriberSet.size(); ++j) {
	    AwdsSubscriber& a = dev->subscriberSet[j];
	    if ( (a.subject == subj) && (a.id == flood.getSrc())) {
		pos = j;
		break;
	    }
	}

	// not found?
	if (pos == -1) {
	    // create and append new entry;
	    AwdsSubscriber a;
	    a.subject = subj;
	    a.id = flood.getSrc();
	    pos  = dev->subscriberSet.size();
	    dev->subscriberSet.push_back(a);

	}

	// update expire time:

	dev->subscriberSet[pos].expire = GEA.lastEventTime + gea::Duration(50.);
	dev->subscriberSet[pos].distance  = initialTTL - flood.getTTL() + 1;
    } // end - for all subjects in the packet.

}




/*
 * \note Everything a little bit crappy/hack-y due to lack of time :(
 */
extern "C"
uint8_t awds_init() {

    REP_MAP_OBJ(awds::Routing *, routing);

    awds_dev_1.routing = routing;
    if (!routing) {
	GEA.dbg() << "AWDS initialisation ... failed" << endl;
	return 1;
    }
    awds_dev_1.init();

    GEA.dbg() << "AWDS initialisation ... success" << endl;
    return 0;

}

/*!
 * \brief AWDS send routine.
 *
 * \param[in] st_msg the message to transmit.
 * \return 0 In case of success,\n
 *                 1 otherwise
 *
 * \note Inlined routine.
 */
extern "C"
uint8_t awds_send(st_message_t* st_msg){

    if (0) {
	printf("Send msg via awds\n");
	printf("Subject: 0x%llx\n", (*st_msg->subject).val);
	printf("DataLength: %d\n", st_msg->data.len);
	for (uint32_t i=0;i<st_msg->data.len;++i)
	    printf("%x",st_msg->data.val[i]);
	printf("\n");
    }
    const uint64_t subj = (*st_msg->subject).val;

    // build up list of receivers.
    vector<awds::NodeId> receivers;
    bool all_reachable = true;

    for (int i = 0; i < awds_dev_1.subscriberSet.size() ; ++i) {

	if ( (awds_dev_1.subscriberSet[i].subject == subj) &&
	     (awds_dev_1.subscriberSet[i].expire > GEA.lastEventTime) ) {

	    awds::NodeId receiver = awds_dev_1.subscriberSet[i].id;
	    receivers.push_back(receiver);
	    all_reachable = all_reachable && awds_dev_1.routing->isReachable(receiver);

	    GEA.dbg() << "found receiver " << receiver
		      << " distance="  << awds_dev_1.subscriberSet[i].distance
		      << endl;

	}
    }

    // transmit, but check before, if there are subscribers.

    if (receivers.size() == 0 ) {
	GEA.dbg() << "Not sending as there are no subscribers" << endl;

    } else if ( (receivers.size() <= awds_dev_1.max_uc_receivers) && all_reachable ){

	// is it more efficient to use unicast?

	GEA.dbg() << "Only " << receivers.size() << " receivers, sending via unicast" << endl;

	for (int i = 0; i < receivers.size(); ++i) {

	    awds::BasePacket *p = awds_dev_1.routing->newUnicastPacket(AWDS_UC_PUBLISH_TYPE);
	    awds::UnicastPacket uniP(*p);
	    uniP.setUcDest(receivers[i]);

	    toArray<uint64_t>(subj, p->buffer +
			      awds::UnicastPacket::UnicastPacketEnd);
	    ::memcpy(p->buffer + awds::UnicastPacket::UnicastPacketEnd + 8,
		     st_msg->data.val,
		     st_msg->data.len);

	    p->size = awds::UnicastPacket::UnicastPacketEnd + 8 + st_msg->data.len;

	    awds_dev_1.routing->sendUnicast(p);
	    p->unref();
	}

    } else {

	// we have to use the inefficient flooding.

	awds::BasePacket *p = awds_dev_1.routing->newFloodPacket(AWDS_BC_PUBLISH_TYPE);
	awds::Flood flood(*p);

	flood.setTTL(15);


	toArray<uint64_t>(subj, p->buffer + awds::Flood::FloodHeaderEnd);
	::memcpy(p->buffer + awds::Flood::FloodHeaderEnd + 8, st_msg->data.val, st_msg->data.len);
	p->size = awds::Flood::FloodHeaderEnd + 8 + st_msg->data.len;

	awds_dev_1.routing->sendBroadcast(p);

	p->unref();
    }



    return 0;
}


extern "C"
uint8_t awds_bind_announce(subject_t* subject) {

    if (!awds_dev_1.bound_pub) {
	awds_dev_1.bound_pub = true;
	awds_dev_1.routing->registerBroadcastProtocol(AWDS_BC_SUBSCRIBE_TYPE,
						      awds_receive_subscription,
						      (void*)&awds_dev_1);
    }

    return 0;
}

static void updateSubscription(gea::Handle *h, gea::AbsTime t, void *data) {

    struct awds_nic_dev *dev = static_cast<struct awds_nic_dev *>(data);

    if (dev->subscriptions.empty()) {
	// stop periodic subscription if there are no subscribers.
	awds_dev_1.subscribing = 0;
	return;
    }

    // prepare and send a flood packet;

    awds::BasePacket *p = dev->routing->newFloodPacket(AWDS_BC_SUBSCRIBE_TYPE);
    awds::Flood flood(*p);

    uint32_t initTTL = dev->subTTL;
    flood.setTTL(initTTL);

    GEA.dbg() << "updateing subscr. with ttl=" << initTTL << endl;

    char *ptr = p->buffer + awds::Flood::FloodHeaderEnd;
    toArray<uint32_t>(initTTL, ptr);
    ptr += 4;

    const uint16_t num = dev->subscriptions.size();
    toArray<uint16_t>(num, ptr);
    ptr += 2;

    for (uint16_t i = 0; i < num; ++i) {
	toArray<uint64_t>(dev->subscriptions[i], ptr);
	ptr += 8;
    }

    p->size = awds::Flood::FloodHeaderEnd + 2 + 4 + (8*num);
    dev->routing->sendBroadcast(p);

    // schedule next period...
    GEA.waitFor(h, t + dev->subscr_period, updateSubscription, data);

}


extern "C"
uint8_t awds_bind_subscribe(subject_t* subject) {

    if (!awds_dev_1.listening) {
	awds_dev_1.routing->registerBroadcastProtocol(AWDS_BC_PUBLISH_TYPE, awds_receive_bc, 0);
	awds_dev_1.routing->registerUnicastProtocol(AWDS_UC_PUBLISH_TYPE, awds_receive_uc, 0);

	awds_dev_1.listening = 1;
    }

    awds_dev_1.subscriptions.push_back(subject->val); // add to the list of subscriptions.

    if (!awds_dev_1.subscribing) {
	// start the periodic subscription
	awds_dev_1.subscribing = 1;
	updateSubscription( &awds_dev_1.blocker,
			    gea::AbsTime::now(),
			    (void *)&awds_dev_1 );
    }

    return 0;
}

extern "C"
void    awds_unbind_announce(subject_t* subject) {
    // \todo implement this.
}

extern "C"
void    awds_unbind_subscribe(subject_t* subject) {
    // \todo implement this.
}



/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */
