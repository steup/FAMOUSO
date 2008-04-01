/* $Id$ */
#ifndef __Chain_h__
#define __Chain_h__

//
//	Chain abstraction. This class is used to implement various chains
//	of objects. A chain object is a container that carries an address
//	of a chain object.
//

class Chain{
	Chain* link;				// successor element
public:
	Chain ();
	Chain (Chain*);

	Chain* select () const;			// return successor
	void   select (Chain*);			// define successor

	void   append (Chain&); 		// add successor

	Chain* ending () const;			// return tail item
	Chain* search (const Chain*) const;	// return container instance
	Chain* unlink ();			// remove and return successor
	Chain* remove ();			// remove all, return successor
	void   remove (const Chain&);		// remove item from chain
	void   detach ();			// remove successor
};

#include"inline/Chain.cc"

#endif
