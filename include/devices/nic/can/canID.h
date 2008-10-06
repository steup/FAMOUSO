#ifndef __canID_h__
#define __canID_h__

namespace famouso {
  namespace mw {
    namespace nl {
      namespace CAN {
	namespace detail {

	  // little endian defined ID
	  union __attribute__((packed)) ID {
	  private:
	    uint32_t _value;
	    uint8_t _v[4];
	  class __attribute__((packed)) {
	    public:
	      uint16_t _etag      : 14;
	      uint8_t  _tx_nodelo :  2;
	      uint8_t  _tx_nodehi :  5;
	      int8_t   _priolo    :  3;
	      int8_t   _priohi    :  5;
	    } parts;
	    class __attribute__((packed)) {
	    public:
	      uint16_t _etag      : 14;
	      uint8_t  _nibblelo  :  2;
	      uint8_t  _nibblehi  :  2;
	      uint8_t  _stage     :  4;
	    } parts_ccp;
	  public:

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

	    uint8_t ccp_nibble() {
	      return parts_ccp._nibblelo | (parts_ccp._nibblehi << 2) ;
	    }

	    uint8_t & operator[](uint8_t i) {
	      return _v[i];
	    }

	    void value(uint32_t v) {
	      _value = v & 0x1fffffff;
	    }

	    uint32_t value() {
	      return _value;
	    }
	  };

	} /* namespace detail */
      } /* namespace CAN */
    } /* namespace nl */
  } /* namespace mw */
} /* namespace famouso */

#endif
