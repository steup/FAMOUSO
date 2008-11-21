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

  /*!
  * \brief new ETAG binding protocol
  *
  * \todo busy waiting in the bind_subject method at the moment
  *	  but we have to block until the subject is bound. That
  *	  could lead to a problem in case of using the client on
  *	  a gateway node, because the waiting will be done within
  *	  a asynchrone function call and it blocks the rest of the
  *	  system (asio) till it is finished. For that purpose it
  *	  needs a better solution.
  *
  * \todo at the moment the received broker answer is not checked
  *	  whether the etag corresponds the the subject
  */
template < class CAN_Driver, typename ID = famouso::mw::nl::CAN::detail::ID >
class Client {
	bool passed;
	uint16_t	etag;
 public:

 Client(): passed(false), etag(0) {}
 /*! \brief Bind a Subject to an etag.
  *
  *  \param[in] sub the Subject that should be bound
  *  \return the bound etag in case of success else 0
  *
  */
 uint16_t bind_subject(const Subject &sub, uint16_t tx_node, CAN_Driver& canDriver) {
   typename CAN_Driver::MOB mob;
   ID *id = reinterpret_cast<ID*>(&mob.id());
   mob.len(8);
   for (uint8_t i=0;i<mob.len();++i)
     mob.data()[i] = sub.tab[i];

   id->prio(0xFD);
   id->etag(famouso::mw::nl::CAN::ETAGS::GET_ETAG);
   id->tx_node(tx_node);
   canDriver.send(mob);

	while(!passed);
	passed=false;
   return etag;

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
            ID *id = &mob.id();
            if ( id->etag() == famouso::mw::nl::CAN::ETAGS::SUPPLY_ETAG_NEW_BP ) {
		etag=mob.data()[3];
		passed=true;
		return true;
	    } else {
		return false;
	    }
        }
};

}
} /* namespace CAN */
} /* namespace nl */
} /* namespace mw */
} /* namespace famouso */

#endif

