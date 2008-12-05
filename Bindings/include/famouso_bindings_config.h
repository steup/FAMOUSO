#ifndef _famouso_bindings_h_
#define _famouso_bindings_h_

#include "mw/el/EventLayerClientStub.h"
#include "mw/api/EventChannel.h"
#include "mw/api/PublisherEventChannel.h"
#include "mw/api/SubscriberEventChannel.h"

#include "mw/common/Event.h"
#include "famouso.h"

namespace famouso {
namespace config {
typedef famouso::mw::el::EventLayerClientStub ECH;
typedef famouso::mw::api::PublisherEventChannel  < ECH > PEC;
typedef famouso::mw::api::SubscriberEventChannel < ECH > SEC;
}}

#endif
