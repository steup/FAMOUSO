#ifndef __etagBP_h__
#define __etagBP_h__

#include "devices/nic/can/peak/PeakCAN.h"
#include "mw/nl/can/constants.h"
#include "mw/nl/can/canETAGS.h"
#include "mw/nl/can/canID.h"
#include "mw/common/UID.h"

namespace famouso {
  namespace mw {
    namespace nl {
      namespace CAN {

namespace etagBP {
// old ETAG supply protocol
template < class CAN_MOB, typename ID=famouso::mw::nl::CAN::detail::ID>
class Broker {

        UID etags[constants::etagBP::count];

		uint16_t bind_etag(UID &uid) {
			uint16_t etag=0;
			while (etag < constants::etagBP::count) {
                if ( (etags[etag] == 0 ) || ( etags[etag] == uid ) )
					break;
				++etag;
			}
			return etag;
		}

    public:
        Broker() {
            for ( uint16_t i = 0; i < constants::etagBP::count; ++i ) {
                etags[i].value = 0;
            }
        }

		uint16_t bind(UID &uid) {
			return bind_etag(uid) & 0x3fff;
		}

        bool get_etag(CAN_MOB &mob) {
			ID *id = reinterpret_cast<ID*>(&mob.ID);
			UID uid(*reinterpret_cast<UID*>(mob.DATA));

			uint16_t etag=0;
			while (etag < constants::etagBP::count) {
                if ( (etags[etag] == 0 ) || ( etags[etag] == uid ) )
					break;
				++etag;
			}
            if ( etag ==  constants::etagBP::count) {
				std::cout << "no free etags -- that should never be happen" << std::endl;
				return false;
			} else {
				etags[etag]=uid;
				etag+=constants::etagBP::reserved;
				mob.LEN=4;
				mob.DATA[0] = id->tx_node();
				mob.DATA[1] = 0x3;
				mob.DATA[2] = etag >> 8;
				mob.DATA[3] = static_cast<uint8_t>(etag & 0xff);
                id->etag(famouso::mw::nl::CAN::ETAGS::SUPPLY_ETAG);
				id->tx_node(constants::Broker_tx_node);
				std::cout << "Supply etag\t -- UID [0x" << std::hex << uid.value << "]"
						  << " -> etag [0x" << etag << "]" << std::endl;
				return true;
			}
         }
};


template < class CAN_MOB, typename ID=famouso::mw::nl::CAN::detail::ID>
class Client {
	public:
		uint16_t bind(UID &uid) {
			return 1000;
		}
};

}
} /* namespace CAN */
    } /* namespace nl */
  } /* namespace mw */
} /* namespace famouso */

#endif

