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



/* #define MAX_EVENT_DATA_LENGTH	8 */

/* typedef unsigned long long famouso_subject_t; */

/* typedef struct { */
/* 	famouso_subject_t subject; */
/* 	int len; */
/* 	char data[MAX_EVENT_DATA_LENGTH]; */
/* } famouso_event_t; */

/* int famouso_announce(famouso_subject_t subject); */
/* int famouso_publish(famouso_event_t event); */

/* int famouso_subscribe(famouso_subject_t subject); */
/* int famouso_poll(famouso_event_t *event); */
/* int famouso_unsubscribe(famouso_subject_t subject); */

#endif
