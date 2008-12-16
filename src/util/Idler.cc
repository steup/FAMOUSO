#include "util/Idler.h"

#include <boost/thread.hpp>
#include <boost/thread/xtime.hpp>

#include <signal.h>

namespace Idler {
    volatile bool done=false;
    void siginthandler(int) {
        done=true;
    }
    void idle() {
        signal(SIGINT, Idler::siginthandler);
        while(!done){
            boost::xtime time;
            boost::xtime_get( &time, boost::TIME_UTC );
            time.sec += 1;
            boost::thread::sleep( time );
        }
    }
}
