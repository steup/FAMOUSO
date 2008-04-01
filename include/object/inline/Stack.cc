inline Stack::Stack () : Chain() {}

inline Stack::Stack (const Stack& copy) : Chain(copy) {}

inline Stack& Stack::operator = (const Stack& copy) {
	*(Chain*)this = (const Chain&)copy;
	return *this;
}


inline void Stack::insert (Chain& item) {
	item.select(select());		// make head successor
	select(&item);			// define new head
}


inline void Stack::append (Chain& item) {
	register Chain* wrap = search(0);	// find tail
	item.select(0);				// terminate chain
	wrap->select(&item);			// add item
}
