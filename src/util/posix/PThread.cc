#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include "util/Thread.h"

static void* StartThread(void *arg) {
	Thread *p = (Thread *)arg;
	p -> action ();
	delete p;
	return NULL;
}

Thread::Thread() {
}

void Thread::start() {
	pthread_t t;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
	if (pthread_create(&t,&attr, StartThread,this) == -1)
	{
		perror("Thread: create failed");
	}
}

unsigned int Thread::sleep(unsigned int t) {
	return ::sleep(t);
}
Thread::~Thread() {
}
