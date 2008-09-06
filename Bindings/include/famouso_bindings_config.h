#ifndef _famouso_bindings_h_
#define _famouso_bindings_h_

#include "mw/el/EventLayerClientStub.h"
#include "mw/api/EventChannel.h"
#include "mw/api/PublisherEventChannel.h"
#include "mw/api/SubscriberEventChannel.h"

#include "mw/common/Event.h"

typedef famouso::mw::api::EventChannel < famouso::mw::el::EventLayerClientStub > EC;
typedef famouso::mw::api::PublisherEventChannel  < EC > PEC;
typedef famouso::mw::api::SubscriberEventChannel < EC > SEC;

#endif
