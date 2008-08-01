#ifndef __EventLayer_h__
#define __EventLayer_h__

#include "debug.h"

#include "mw/common/Subject.h"
#include "mw/common/Event.h"

#include "object/Queue.h"

// einfacher ANL
template < class LL >
class EventLayer : public LL{
  
    // potentiell gefaerdete Datenstrukturen
    // wegen Nebenlaeufigkeit
    Queue Publisher;
    Queue Subscriber;

public:
    typedef typename LL::SNN SNN;

    void announce(EventChannel<EventLayer> &ec){
	DEBUG(("%s\n", __PRETTY_FUNCTION__));
	LL::announce(ec.subject(), ec.snn());
	Publisher.append(ec);
    }

    void publish(const EventChannel<EventLayer> &ec, const Event &e) {
	DEBUG(("%s %p\n", __PRETTY_FUNCTION__, ec.select()));
	// Hier koennte der Test erfolgen, ob die Subjects vom Event
	// und vom EventChannel gleich sind, weil nur dann
	// das Event in diesen Kanal gehoert
	//
	// Ist mit einem assert zu machen und auch ob das Subject des
	// Kanals ueberhaupt gebunden ist
	LL::publish(ec.snn(),e);

	publish_local(ec,e);
    }

    // hier erfolgt die local Abarbeitung des Kanaele
    // eigentlich der LocalEventChannelHandler
    void publish_local(const EventChannel<EventLayer> &ec, const Event &e) {

	typedef EventChannel<EventLayer> ec_t;
	ec_t* sec= reinterpret_cast<ec_t*>(Subscriber.select());
	while (sec){
	    DEBUG(("%s %p %lld %lld\n", __PRETTY_FUNCTION__, sec, sec->subject().value,ec.subject().value));
	    if (sec->subject() == ec.subject()) 
		sec->callback(e);
	    sec=reinterpret_cast<ec_t*>(sec->select());
	}
    }


    void subscribe(EventChannel<EventLayer> &ec) {
	DEBUG(("%s %p\n", __PRETTY_FUNCTION__, ec.select()));
	Subscriber.append(ec);
	LL::subscribe(ec.subject(), ec.snn());
    }

  
    void unsubscribe(EventChannel<EventLayer> &ec) {
	Subscriber.remove(ec);
	DEBUG(("%s\n", __PRETTY_FUNCTION__));
    }

    void unannounce(EventChannel<EventLayer> &ec) {
	Publisher.remove(ec);
	DEBUG(("%s\n", __PRETTY_FUNCTION__));
    }

private:
    
    // hier muss die Funktion hin, die von unten aufgerufen wird
    // und die entscheidet, ob das event angenommen wird und 
    // welches subject dazu gehoert
    // const subject & find_subjectt_to_snn(snn) const {
    // }
};

#endif /* __EventLayer_h__ */

/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */
