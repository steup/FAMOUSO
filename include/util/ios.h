#ifndef __ios_h__
#define __ios_h__

#include <boost/pool/detail/singleton.hpp>
#include <boost/asio.hpp>


namespace famouso {

    namespace util {

    typedef boost::asio::io_service ios_type;
    typedef boost::details::pool::singleton_default<ios_type> ios;

        namespace impl {
            void start_ios ();
        }
    }
}

#endif
