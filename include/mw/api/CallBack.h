/*******************************************************************************
 *
 * Copyright (c) 2008-2010 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
 * All rights reserved.
 *
 *    Redistribution and use in source and binary forms, with or without
 *    modification, are permitted provided that the following conditions
 *    are met:
 *
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in
 *      the documentation and/or other materials provided with the
 *      distribution.
 *
 *    * Neither the name of the copyright holders nor the names of
 *      contributors may be used to endorse or promote products derived
 *      from this software without specific prior written permission.
 *
 *
 *    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 *    IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 *    TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 *    PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *    OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *
 * $Id$
 *
 ******************************************************************************/

#ifndef __CallBack_h__
#define __CallBack_h__

#include "mw/common/Event.h"
#include "mw/nl/DistinctNL.h"
#include "mw/el/ml/NetworkID.h"

#include "case/Delegate.h"

namespace famouso {
    namespace mw {
        namespace api {

            /*! \brief Definition of the data that the SubscriberEventChannel callback will get as parameter
             */
            typedef const Event SECCallBackData;

            /*! \brief Definition of the callback itself using the CallBackData as template
             */
            typedef famouso::util::Delegate<SECCallBackData&> SECCallBack;


            /*! \brief cb is the default callback
              *
              * \note Probably not needed, because of defining callback by the application
              * itself. However for testing is nice to have. Future versions will remove
              * that function.
              */
            void cb(SECCallBackData& cbd);


            /*! \brief Definition of the data that the EventChannel exception callbacks will get as parameter
             */
            struct ExceptionInfo {
                enum Type {
                    NoEvent,
                    TransmissionFailed,
                    RealTimeReservationFailed,
                };

                Type type;

                /// For publisher exceptions: ID of the network; unused otherwise
                famouso::mw::el::ml::NetworkID network_id;

                ExceptionInfo(Type t, famouso::mw::el::ml::NetworkID net_id) :
                    type(t), network_id(net_id) {
                }
            };

            /*! \brief Definition of exception callback delegate type
             */
            typedef famouso::util::Delegate<const ExceptionInfo&> ExceptionCallBack;

            /*! \brief ecb is the default exception callback
              *
              * \note Probably not needed, because of defining callback by the application
              * itself. However for testing is nice to have. Future versions will remove
              * that function.
              */
            void ecb(const ExceptionInfo &);


        }
    }
}

#endif /* __CallBack_h__ */
