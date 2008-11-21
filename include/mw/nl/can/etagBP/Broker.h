#ifndef __etagBP_Broker_h__
#define __etagBP_Broker_h__

#include "devices/nic/can/peak/PeakCAN.h"
#include "mw/nl/can/constants.h"
#include "mw/nl/can/canETAGS.h"
#include "mw/nl/can/canID.h"
#include "mw/common/Subject.h"
#include "util/endianess.h"

namespace famouso {
namespace mw {
namespace nl {
namespace CAN {
namespace etagBP {

// old ETAG supply protocol
template < class CAN_Driver, typename ID = famouso::mw::nl::CAN::detail::ID >
class Broker {

        Subject etags[constants::etagBP::count];

        /*! \brief Bind a Subject to an etag.
         *
         *  \param[in] sub the Subject that should be bound
         *  \return the bound etag in case of success else 0
         */
        uint16_t bind_subject_to_etag(const Subject &sub) {
            uint16_t etag = constants::etagBP::count;
            while (etag > constants::etagBP::reserved) {
                --etag;
                if ( (etags[etag] == 0 ) || ( etags[etag] == sub ) ) {
                    etags[etag] = sub;
                    std::cout << "Supply etag\t -- Subject [0x" << std::hex << sub.value << "]"
                    << " -> etag [0x" << etag << "]" << std::endl;
                    return etag;
                }
            }
            std::cout << "no free etags -- that should never be happen" << std::endl;
            return 0;
        }

    public:
        Broker() {
            for ( uint16_t i = 0; i < constants::etagBP::count; ++i ) {
                etags[i].value = 0;
            }
        }

        /*! \brief Bind a Subject to an etag.
         *
         *  \param[in] sub the Subject that should be bound
         *  \return the bound etag in case of success else 0
         */
	uint16_t bind_subject(const Subject &sub, uint16_t tx_node, CAN_Driver& canDriver) {
            return bind_subject_to_etag(sub);
        }

        /*! \brief handle a Subject bind request.
         *
         *  \param[in] mob the CAN message that contains the
         *             Subject that should be bound
         *  \param[in] canDriver the driver which is used to
         *             deliver answer to the request
         *
         *  \return true if it a binding was succeeded and false
         *          in case the message wasn't a binding request
         */
        bool handle_subject_bind_request(typename CAN_Driver::MOB &mob, CAN_Driver& canDriver) {
            ID *id = &mob.id();
            Subject sub(htonll(*reinterpret_cast<uint64_t*>(mob.data())));
            if ( id->etag() == famouso::mw::nl::CAN::ETAGS::GET_ETAG ) {
                uint16_t etag = bind_subject(sub,constants::Broker_tx_node, canDriver);
                mob.len(4);
                mob.data()[0] = id->tx_node();
                mob.data()[1] = 0x3;
                mob.data()[2] = etag >> 8;
		mob.data()[3] = static_cast<uint8_t>(etag & 0xff);
                id->prio(0xFD);
                id->etag(famouso::mw::nl::CAN::ETAGS::SUPPLY_ETAG);
                id->tx_node(constants::Broker_tx_node);
                canDriver.send(mob);

                mob.len(8);
                mob.data()[0] = id->tx_node();
                mob.data()[1] = 0x3;
                mob.data()[2] = etag >> 8;
                mob.data()[3] = static_cast<uint8_t>(etag & 0xff);
                mob.data()[4] = sub.tab[0];
                mob.data()[5] = sub.tab[1];
                mob.data()[6] = sub.tab[2];
                mob.data()[7] = sub.tab[3];
                id->etag(famouso::mw::nl::CAN::ETAGS::SUPPLY_ETAG_NEW_BP);
                id->tx_node(constants::Broker_tx_node);
                canDriver.send(mob);

                mob.data()[4] = sub.tab[4];
                mob.data()[5] = sub.tab[5];
                mob.data()[6] = sub.tab[6];
                mob.data()[7] = sub.tab[7];
                canDriver.send(mob);

                return true;
            }
            return false;
        }
};

}
} /* namespace CAN */
} /* namespace nl */
} /* namespace mw */
} /* namespace famouso */

#endif

