#include "util/Idler.h"
#include "util/ios.h"

#include <boost/thread.hpp>
#include <boost/thread/xtime.hpp>

#include <signal.h>

namespace Idler {
    volatile bool done=false;
    void siginthandler(int) {
        // signalise the ios-thread to stop
        famouso::util::ios::instance().stop();
        done=true;
    }
    void idle() {
        signal(SIGINT, Idler::siginthandler);
        boost::xtime time;
        while(!done){
            boost::xtime_get( &time, boost::TIME_UTC );
            time.nsec += 500000;
            boost::thread::sleep( time );
        }
        // wait an additional short time to ensure that the
        // ios-thread has finished
        boost::xtime_get( &time, boost::TIME_UTC );
        time.nsec += 100000;
        boost::thread::sleep( time );
    }
}
