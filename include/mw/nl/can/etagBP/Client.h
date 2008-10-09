#ifndef __etagBP_Client_h__
#define __etagBP_Client_h__

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
class Client {
    public:

        /*! \brief Bind a Subject to an etag.
         *
         *  \param[in] sub the Subject that should be bound
         *  \return the bound etag in case of success else 0
         *
         * \todo genau nachdenken, wie man das Bind im Client
         *       nebenlaeufig und eindeutig durchfuehrt.
         */
        uint16_t bind_subject(const Subject &sub) {
//            typename CAN_Driver::MOB mob;
//            ID *id = reinterpret_cast<ID*>(&mob.ID);
//            mob.DATA = sub;
//            id->etag(famouso::mw::nl::CAN::ETAGS::GET_ETAG );
//            mob.LEN = 8;
//            mob.DATA[0] = id->tx_node();
//            mob.DATA[1] = 0x3;
//            mob.DATA[2] = etag >> 8;
//            mob.DATA[3] = static_cast<uint8_t>(etag & 0xff);
//            id->etag(famouso::mw::nl::CAN::ETAGS::SUPPLY_ETAG);
//            id->tx_node(constants::Broker_tx_node);
//            canDriver.send(mob);
//            std::cout << "Supply etag\t -- Subject [0x" << std::hex << sub.value << "]"
//            << " -> etag [0x" << etag << "]" << std::endl;
//            return true;
            return 1000;
        }

        /*! \brief handle a Subject bind request.
         *
         *  \param[in] mob the CAN message that contains the
         *             Subject that should be bound
         *  \param[in] canDriver the driver which is used to
         *             deliver answer to the request
         *
         *  \return always false, because the client can not handle
         *          such requests. This allows the compiler further
         *          optimizations and in the best case the code is
         *          complete removed.
         */
        bool handle_subject_bind_request(typename CAN_Driver::MOB &mob, CAN_Driver& canDriver) {
            return false;
        }
};

}
} /* namespace CAN */
} /* namespace nl */
} /* namespace mw */
} /* namespace famouso */

#endif

