#include "famouso_bindings.h"

//  Events are stored in a simple cyclic buffer
//  if the buffer is full, events get lost

// SubscriberEC code
SubscriberEC::SubscriberEC(const subject_t &sub, uint32_t count) :first(0), last(0), max(count) {
	sec=new famouso::config::SEC(sub);
	events = new Storage[count];
}
SubscriberEC::~SubscriberEC() {
	delete sec;
	delete [] events;
}

void SubscriberEC::cb(famouso::mw::api::SECCallBackData &cb) {
	if (!events[first].used) {
		events[first].used=1;
		events[first].event.subject=cb.subject.value;
		events[first].event.len=cb.length;
		memcpy(events[first].event.data,cb.data,cb.length);
		first=(first+1)%max;
	}
}

int SubscriberEC::subscribe() {
	sec->subscribe();
	sec->callback.bind<SubscriberEC, &SubscriberEC::cb>(this);
	return 1;
}

int SubscriberEC::unsubscribe() {
	return 1;
}

// simple automaton for polling the correct event to 
// the application
int SubscriberEC::poll(event_t &e) {
	if (events[last].used) {
		if (events[last].used==2) {
			events[last].used=0;
			last=(last+1)%max;
		}
		if (events[last].used==1) {
			events[last].used++;
			e=events[last].event;
			return 1;
		}
	}
	return 0;
}

// PublisherEC code
PublisherEC::PublisherEC (const subject_t &sub){
	pec=new famouso::config::PEC(sub);
}

PublisherEC::~PublisherEC() {
	delete pec;
}

int PublisherEC::announce(){
	pec->announce();
	return 1;
}

int PublisherEC::unannounce(){
	// needs not to be implemented
	// unannouncement takes place if channel is destructed
	return 1;
}

int PublisherEC::publish(const event_t &event){
	famouso::mw::Event e(event.subject);
	e.length=event.len;
	e.data=reinterpret_cast<uint8_t *>(event.data);
	pec->publish(e);
	return 1;
}

