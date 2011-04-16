
// Real time logging
#define LOGGING_DEFINE_EXTENDED_OUTPUT_TYPE
#define LOGGING_DEFINE_OWN_OUTPUT_TYPE
#include "logging/logging.h"
#include "RealTimeLogger.h"
LOGGING_DEFINE_OUTPUT( ::logging::OutputLevelSwitchDisabled< ::logging::OutputStream< ::logging::RealTimeLogger<false> > > )
//LOGGING_DEFINE_OUTPUT( ::logging::OutputLevelSwitchDisabled< ::logging::OutputStream< ::logging::RealTimeLogger<true> > > )

#include "timefw/TimeSourceProvider.h"
#include "timefw/LocalClock.h"
#include "timefw/ClockDriverPosix.h"
typedef timefw::LocalClock<timefw::ClockDriverPosix> Clock;
FAMOUSO_TIME_SOURCE(Clock)

#include "timefw/Dispatcher.h"

void task() {
    static int i = 0;
    ::logging::log::emit() << i << ' ' << timefw::TimeSource::current() << '\n';
    i++;
}




int main() {

    timefw::Task t;
    //t.period = 10 * 1000 * 1000;        // 1000 ms
    t.period = 10;
    t.start = timefw::TimeSource::current().add_sec(1);
    ::logging::log::emit() << "Task start " << t.start << '\n';
    t.function.bind<&task>();

    timefw::Dispatcher::instance().enqueue(t);
    timefw::Dispatcher::instance().run();

    return 0;
}

