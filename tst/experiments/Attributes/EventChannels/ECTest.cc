#include "logging/logging.h"

#include "famouso.h"

#include "mw/nl/UDPBroadCastNL.h"
#include "mw/anl/AbstractNetworkLayer.h"
#include "mw/el/EventLayer.h"
#include "mw/common/Subject.h"
#include "mw/api/CallBack.h"
#include "mw/common/ExtendedEvent.h"
#include "util/Idler.h"

#include "mw/attributes/Latency.h"
#include "mw/attributes/Throughput.h"
#include "mw/attributes/TTL.h"

#include "mw/api/ExtendedPublisherEventChannel.h"
#include "mw/api/ExtendedSubscriberEventChannel.h"

#include "mw/attributes/detail/AttributeSetProvider.h"
#include "mw/attributes/AttributeSet.h"

// A NL-wrapper defining an attribute provision
template <typename NL>
class ProvidingNL : public NL {
    public:
        typedef famouso::mw::attributes::detail::SetProvider<
                 famouso::mw::attributes::Latency<10>
                >::attrSet attributeProvision;
};

// Works fine, the required latency is provided by the NL
typedef famouso::mw::attributes::detail::SetProvider<
         famouso::mw::attributes::Latency<10>
        >::attrSet req1;

// Will fail, the required latency is stricter than the
//  latency provided by the NL
typedef famouso::mw::attributes::detail::SetProvider<
         famouso::mw::attributes::Latency<5>
        >::attrSet req2;

// Will fail, the NL does not make any statement about the
//  provided throughput
typedef famouso::mw::attributes::detail::SetProvider<
         famouso::mw::attributes::Latency<10>,
         famouso::mw::attributes::Throughput<1000>
        >::attrSet req3;

// Will fail, TTL is not a requirable attribute
typedef famouso::mw::attributes::detail::SetProvider<
         famouso::mw::attributes::Latency<10>,
         famouso::mw::attributes::TTL<5>
        >::attrSet req4;

// The requirement of the publisher event channel
typedef req4 PubReq;

// The requirement of the subscriber event channel
typedef PubReq SubReq;

static const bool compileError = true;

struct config {
    typedef ProvidingNL<famouso::mw::nl::UDPBroadCastNL> nl;
    typedef famouso::mw::anl::AbstractNetworkLayer< nl > anl;
    typedef famouso::mw::el::EventLayer< anl > EL;

    typedef famouso::mw::api::ExtendedPublisherEventChannel<EL, PubReq, compileError> PEC;
    typedef famouso::mw::api::ExtendedSubscriberEventChannel<EL, SubReq, compileError> SEC;
};

typedef config::PEC PEC;
typedef config::SEC SEC;

famouso::mw::Subject subject(0x01);

void cb(famouso::mw::api::SECCallBackData& data) {
    ::logging::log::emit() << "Received event!" << ::logging::log::endl;
}

int main(int argc, char **argv) {
    famouso::init<config>();

    PEC pub(subject);

    SEC sub(subject);

    sub.subscribe();
    sub.callback.bind<&cb>();

    famouso::mw::ExtendedEvent<> e1(pub.subject());

    pub.publish(e1);

    Idler::idle();
    return (0);
}
