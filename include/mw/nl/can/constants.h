#ifndef __constants_h__
#define __constants_h__

namespace famouso {
  namespace mw {
    namespace nl {
      namespace CAN {

	/*! \brief some constants that are used in the CCP */
        struct constants {
	  enum {
	    Broker_tx_node = 0x7f,
	  };
	  // needed constants for the CAN Configuration Protocol
	  struct ccp {
	    enum {
	      count = 0x7f,
	      ccp_stages = 0xf
	    };
	  };
	  // needed constants for the CAN etag Binding Protocol
	  struct etagBP {
	    enum {
	      reserved = 100,
	      count = (1 << 14)-reserved
	    };
	  };
        };

      } /* namespace CAN */
    } /* namespace nl */
  } /* namespace mw */
} /* namespace famouso */


#endif
