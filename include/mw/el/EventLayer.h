#ifndef __EventLayer_h__
#define __EventLayer_h__

#include "debug.h"

#include "mw/common/Subject.h"
#include "mw/common/Event.h"
#include "mw/api/SubscriberEventChannel.h"
#include "mw/api/EventChannel.h"
#include "mw/nl/BaseNL.h"
#include "mw/el/EventLayerCallBack.h"

#include "object/Queue.h"

namespace famouso {
namespace mw {
namespace el {

// EventLayer
template < class LL >
class EventLayer : public LL {

        // potentiell gefaerdete Datenstrukturen
        // wegen Nebenlaeufigkeit
        Queue Publisher;
        Queue Subscriber;

        void* operator new(size_t, void* __p) { return __p; }

    public:
        void init() {
            // late/defered Constructor call on pre-allocated memory
            // using the placement new of C++
            new (this) EventLayer;
            // initialisierungscode
            // z.B. Callbacks setzen und in allen
            // Netzen registrieren, CCP in unteren Layer, etc.
            IncommingEventFromNL.bind<EventLayer<LL>, &EventLayer<LL>::fetch >(this);
            LL::init();
        }

        typedef typename LL::SNN SNN;

        void announce(famouso::mw::api::EventChannel<EventLayer> &ec) {
            DEBUG(("%s\n", __PRETTY_FUNCTION__));
            LL::announce(ec.subject(), ec.snn());
            Publisher.append(ec);
        }

        void publish(const famouso::mw::api::EventChannel<EventLayer> &ec, const Event &e) {
            DEBUG(("%s %p\n", __PRETTY_FUNCTION__, ec.select()));
            // Hier koennte der Test erfolgen, ob die Subjects vom Event
            // und vom EventChannel gleich sind, weil nur dann
            // das Event in diesen Kanal gehoert
            //
            // Ist mit einem assert zu machen und auch ob das Subject des
            // Kanals ueberhaupt gebunden ist
            //
            // aber diese Test koennten auch direkt im PublisherEventChannel gemacht werden
            LL::publish(ec.snn(),e);

            publish_local(ec,e);
        }

        void subscribe(famouso::mw::api::EventChannel<EventLayer> &ec) {
            DEBUG(("%s %p\n", __PRETTY_FUNCTION__, ec.select()));
            Subscriber.append(ec);
            LL::subscribe(ec.subject(), ec.snn());
        }

        void unsubscribe(famouso::mw::api::EventChannel<EventLayer> &ec) {
            Subscriber.remove(ec);
            DEBUG(("%s\n", __PRETTY_FUNCTION__));
        }

        void unannounce(famouso::mw::api::EventChannel<EventLayer> &ec) {
            Publisher.remove(ec);
            DEBUG(("%s\n", __PRETTY_FUNCTION__));
        }

    private:

        // hier erfolgt die local Abarbeitung des Kanaele
        // eigentlich der LocalEventChannelHandler
        void publish_local(const famouso::mw::api::EventChannel<EventLayer> &ec, const Event &e) {

            typedef famouso::mw::api::SubscriberEventChannel< EventLayer >ec_t;
            ec_t* sec= reinterpret_cast<ec_t*>(Subscriber.select());
            while (sec) {
                DEBUG(("%s %p %lld %lld\n", __PRETTY_FUNCTION__, sec, sec->subject().value,ec.subject().value));
                if ( sec->subject() == ec.subject() )
                    sec->callback(e);
                sec=reinterpret_cast<ec_t*>(sec->select());
            }
        }

        // hier muss die Funktion hin, die von unten aufgerufen wird
        // und die entscheidet, ob das event angenommen wird und
        // welches subject dazu gehoert
        // const subject & find_subject_to_snn(snn) const {
        // }

    public:
        void fetch(famouso::mw::nl::BaseNL *bnl=0) {
            // Frage: Was muss fetch alles durchfuehren
            //	-unteren Layer fragen, nach einem event
            //	-dazu muss:
            //		a) der richtige Layer gefunden werden
            //		b) die SubscriberListe mit durchgereicht werden, um
            //		   matching zwischen SNN und Subject zu ermoeglichen
            //		c) Referenz oder Pointer auf ein Event gegeben werden
            //
            DEBUG(("%s\n", __PRETTY_FUNCTION__));
            typedef famouso::mw::api::SubscriberEventChannel< EventLayer >ec_t;
            ec_t* sec= reinterpret_cast<ec_t*>(Subscriber.select());
            while(sec) {
//            if (!sec)
//                DEBUG(("No Subscribers\n"));
//            else{
//                do {
//                DEBUG(("%s %p %lld %lld\n", __PRETTY_FUNCTION__, sec, sec->subject().value,ec.subject().value));

                    /*! \todo The fetch and the getEvent method should be pooled
                     */
                    if (LL::fetch(sec->snn(), bnl)) {// vergleich der Subjects
                        // versuchen das Event zu holen
                        // finden wir eins, wird es local gepublished, welches
                        // das verteilen an alle Subscriber bewirkt.
                        Event e(sec->subject());
                        LL::getEvent(e);
                        publish_local(*sec,e);
                        return;
                    }
                sec=reinterpret_cast<ec_t*>(sec->select());
//                } while(sec);
            }
            LL::event_processed();
        }
};

} // namespace el
} // namespace mw
} // namespace famouso

#endif /* __EventLayer_h__ */

