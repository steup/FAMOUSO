#include "boost/thread.hpp"
#include "util/ios.h"
#include <iostream>

namespace famouso {
    namespace util {
        namespace impl {
            void start_ios(){
                static boost::asio::io_service::work work(famouso::util::ios::instance());
                static boost::thread t(boost::bind(&famouso::util::ios_type::run,
                                                   &famouso::util::ios::instance()));
            }
        }
    }
}

