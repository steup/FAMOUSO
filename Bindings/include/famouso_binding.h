#ifndef _FAMOUSO_PY_H_
#define _FAMOUSO_PY_H_

#include <sys/types.h>

#define MAX_EVENT_DATA_LENGTH	8

typedef unsigned long long famouso_subject_t;

typedef struct {
	famouso_subject_t subject;
	int len;
	char data[MAX_EVENT_DATA_LENGTH];
} famouso_event_t; 

int famouso_announce(famouso_subject_t subject);
int famouso_subscribe(famouso_subject_t subject);
int famouso_unsubscribe(famouso_subject_t subject);
int famouso_publish(famouso_event_t event);
int famouso_poll(famouso_event_t *event);

#endif
