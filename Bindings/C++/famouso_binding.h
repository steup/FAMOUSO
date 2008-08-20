#ifndef _famouso_bindings_h_
#define _famouso_bindings_h_

#include "mw/el/EventLayerClientStub.h" 
#include "mw/api/EventChannel.h"
#include "mw/api/PublisherEventChannel.h"
#include "mw/api/SubscriberEventChannel.h"

#include "mw/common/Event.h"

typedef EventChannel < EventLayerClientStub > EC;
typedef PublisherEventChannel  < EC > PEC;
typedef SubscriberEventChannel < EC > SEC;

#endif
