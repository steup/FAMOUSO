#include <windows.h>
#include <process.h>    /* _beginthread, _endthread */
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include "util/Thread.h"

static void StartThread(void *arg) {
	Thread *p = (Thread *)arg;
	p -> action ();
	delete p;
	_endthreadex(0);
}

Thread::Thread() {
}

void Thread::start() {
  if( (HANDLE)_beginthread(&StartThread,0, this) < 0)
    perror("Thread creation failed.\n");
}

unsigned int Thread::sleep(unsigned int t) {
	::Sleep(((long)t)*1000);
	return t;
}


Thread::~Thread() {
}


