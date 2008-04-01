#include "assert.h"

inline Chain::Chain () { link = 0; }

inline Chain::Chain (Chain* item) { link = item; }

//
//	Return next chain item.
//
inline Chain* Chain::select () const { return link; }

//
//	Define next item according to actual parameter.
//
inline void Chain::select (Chain* item) { link = item; }

//
//	Detach next item from this chain instance.
//
inline void Chain::detach () {
	assert(select());
	select(select()->select());
}

//
//	Append specified item at this chain instance.
//
inline void Chain::append (Chain& item) {
	item.select(select());		// append item at this item
	select(&item);			// make item new successor
}

//
//	Unlink next item from chain. Update next pointer.
//
inline Chain* Chain::unlink () {
	register Chain* item;
	if ((item = select())!=0) {	// there is a successor, remove...
		detach();
	}
	return item;
}

//
//	Search container instance of specified chain item. Once found,
//	pointer to the container instance is returned. Otherwise, null
//	will be delivered.
//
inline Chain* Chain::search (register const Chain* item) const {
	register Chain* wrap = (Chain*)this;

	while (wrap && (wrap->select() != item)) {
		wrap = wrap->select();
	}

	return wrap;
}

//
//	Remove entire chain and deliver head/next pointer.
//
inline Chain* Chain::remove () {
	register Chain* item = select();
	select(0);
	return item;
}

//
//	Remove specified item from the chain.
//
inline void Chain::remove (const Chain& seek) {
	register Chain* wrap = search(&seek);
	if (wrap) {			// present, unlink from chain
		wrap->detach();
	}
}

//
//	Deliver ending of this list of chain items.
//
inline Chain* Chain::ending () const {
	register Chain* item = (Chain*)this;
	while (item->select()) {
		item = item->select();
	}
	return item;
}




