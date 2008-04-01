#ifndef __Stack_h__
#define __Stack_h__

#include "object/Chain.h"

//
//	Stack abstraction. The stack is implemented as a single-linked
//	list of chain elements. Top of stack is realized as chain item
//	too. Thus, stacks can be chained.
//

class Stack : public Chain {
public:
	Stack ();
	Stack (const Stack&);

	Stack& operator = (const Stack&);

	void append (Chain&); 	// add tail element
	void insert (Chain&);	// add head element (push)
};

#include "inline/Stack.cc"
#endif
