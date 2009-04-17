#ifndef __UDPBroadCastNL_h__
#define __UDPBroadCastNL_h__

#include "util/ios.h"
#include "mw/nl/BaseNL.h"
#include "mw/nl/Packet.h"
#include "mw/common/NodeID.h"
#include "mw/common/Subject.h"
#include "mw/api/EventChannel.h"
#include "mw/el/EventLayerCallBack.h"
#include "debug.h"
#include <stdio.h>

namespace famouso {
    namespace mw {
        namespace nl {

            class UDPBroadCastNL : public BaseNL {
                public:
                    struct info {
                        enum {
                            payload = 8
                        };
                    };

                    typedef int32_t SNN;

                    typedef Packet<SNN> Packet_t;


                    UDPBroadCastNL() { }
                    ~UDPBroadCastNL() {}

                    void init(const NodeID &i) {
                        DEBUG(("%s Configuration 64Bit NodeID=%lld\n", __PRETTY_FUNCTION__, i.value));
                    }

                    // bind Subject to specific networks name
                    void bind(const Subject &s, SNN &snn) {
                        DEBUG(("%s\n", __PRETTY_FUNCTION__));
                    }

                    void deliver(const Packet_t& p) {
                        DEBUG(("%s\n", __PRETTY_FUNCTION__));
                    }

                    void deliver_fragment(const Packet_t& p) {
                        DEBUG(("%s\n", __PRETTY_FUNCTION__));
                    }

                    void fetch(Packet_t& p) {
                        DEBUG(("%s\n", __PRETTY_FUNCTION__));
                    }

                    void interrupt() {
                        DEBUG(("%s\n", __PRETTY_FUNCTION__));
                    }

                    void init() {
                        famouso::util::impl::start_ios();
                        DEBUG(("%s\n", __PRETTY_FUNCTION__));
                    }
                    SNN lastPacketSNN() {
                        return 0;
                    }
            };
        }
    }
}
#endif /* __UDPBroadCastNL_h__ */

