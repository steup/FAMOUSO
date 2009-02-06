#include "boost/thread.hpp"
#include "util/ios.h"
#include <iostream>

namespace famouso {
    namespace util {
        namespace impl {
            namespace detail {
                void run() {
                    try {
                        boost::asio::io_service::work work(famouso::util::ios::instance());
                       famouso::util::ios::instance().run();
                    } catch (const char *c ) {
                        std::cerr << "Exception : " << c << std::endl;
                        abort();
                    }
                }
            }
            void start_ios(){
                    static boost::thread t(boost::bind(&famouso::util::impl::detail::run));
            }
        }
    }
}

