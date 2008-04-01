#include <stdio.h>
#include <unistd.h>
#include "util/Thread.h"

class T : public Thread {
public:

	void action () {
		printf("I'm a thread\n");
		sleep(1);
	}
};

int main(int argc, char** argv){
	printf("create thread\n");
	T t;
	printf("start thread\n");
	t.start();
	printf ("wait for 5s\n");
	Thread::sleep(5);
}
