#ifndef __Queue_h__
#define __Queue_h__

#include "object/Stack.h"

//
//	Abstraction from a queue. The tail pointer of the queue
//	descriptor is a pointer to a chain element. This way, a
//	queue chain element can be efficiently inserted without
//	keeping track of the queue head pointer. The queue must
//	be properly constructed to let the tail pointing to the
//	head. Only the dequeueing of elements is responsible to
//	check the state of the head pointer and adjust the tail
//	if the last element was removed. The queue supports the
//	two strategies LIFO and FIFO. The LIFO strategy means a
//	stack abstraction. Therefore the queue is also a stack.
//	Because stacks can be chained and queues are customized
//	(i.e., specialized) stacks, queues are chainable too.
//

class Queue : public Stack {
	Chain*	tail;			// where to append elements
protected:
	void ending (const Chain&);	// define tail pointer
public:
	Queue ();
	Queue (const Queue&);

	Queue& operator = (const Queue&);

	void   couple (Chain&); 	// add tail element
	void   append (Chain&); 	// couple item at tail, terminate list
	void   insert (Chain&);		// add head element

	void   append (const Queue&); 	// add queue at tail
	void   insert (const Queue&);	// add queue at head

	void   adjust (); 		// set tail onto head

	Chain* ending () const;		// return tail pointer

	Chain* unlink ();		// remove and return head, adjust tail
	Chain* remove ();		// remove all and return head
	void   remove (const Chain&);	// remove specified element
	void   detach ();		// remove head element

	int    intact () const;		// return true if queue is intact
};

#include "inline/Queue.cc"

#endif
