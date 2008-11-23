#ifndef __ios_h__
#define __ios_h__

#include <boost/pool/detail/singleton.hpp>
#include <asio.hpp>


namespace famouso {

    typedef asio::io_service ios_type;
    typedef boost::details::pool::singleton_default<ios_type> ios;

}

#endif
