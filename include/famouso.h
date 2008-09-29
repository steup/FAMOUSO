#ifndef __famouso_h__
#define __famouso_h__

struct FAMOUSO {
  enum {
    SUBSCRIBE=0x50,
    UNSUBSCRIBE,
    PUBLISH=0x52,

    ANNOUNCE=0x56
  };
};

// definition des lokelen Ports, wo der ECH lauscht
#define ServPort 5005
// mit dem localen ECH verbinden.
#define servAddress "127.0.0.1"

// Buffergroesse, beschreibt maximale Nachrichtengroesse.
// sollte vielleicht nicht so fest geschrieben werden.
#define BUFSIZE 65535


// Initalisierungsfunktion, die mit dem richtigen Template
// aufgerufen werden muss, welches der ECH ist.
namespace famouso {
template <class T>
inline void init () {
	T::ech().init();
}
}

#endif
