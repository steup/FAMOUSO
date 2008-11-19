#ifndef __canETAGS_h__
#define __canETAGS_h__

namespace famouso {
  namespace mw {
    namespace nl {
      namespace CAN {
	
	/*! \brief Predefined etags for ccp and etag exchange */
	namespace ETAGS {
	  enum {
	    CCP_SSI		= 0x0,
	    CCP_RSI,
	    GET_ETAG,
	    SUPPLY_ETAG,
	    SUPPLY_ETAG_NEW_BP
	  };

	} /* namespace ETAGS */
      } /* namespace CAN */
    } /* namespace nl */
  } /* namespace mw */
} /* namespace famouso */

#endif
