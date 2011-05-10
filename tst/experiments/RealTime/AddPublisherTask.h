/*******************************************************************************
 *
 * Copyright (c) 2011 Philipp Werner <philipp.werner@st.ovgu.de>
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


#ifndef __ADDPUBLISHERTASK_H_EF4BA438DDB920__
#define __ADDPUBLISHERTASK_H_EF4BA438DDB920__


#include "debug.h"
#include "timefw/Time.h"
#include "timefw/Dispatcher.h"
#include "mw/common/Subject.h"
#include "mw/api/CallBack.h"
#include "mw/el/ml/LocalChanID.h"


template <typename EC>
class AddPublisherTask : public EC {
        typedef AddPublisherTask type;
        typedef EC Base;

    public:
        /*!
         *  \brief  Constructor
         */
        AddPublisherTask(const famouso::mw::Subject& subject,
                         const timefw::Time pub_task_start = timefw::TimeSource::current()) :
                Base(subject),
                publisher_task(pub_task_start, timefw::Time::usec(Base::period), true)
        {
            // Bind empty implementation (should be overwritten by application)
            publisher_task.template bind < &famouso::mw::api::ecb >();

            timefw::Dispatcher::instance().enqueue(publisher_task);
            ::logging::log::emit()
                << "[ RT PUB ] start publisher task: chan "
                << famouso::mw::el::ml::getLocalChanID(this)
                << " at " << publisher_task.start << '\n';
        }

        timefw::Task publisher_task;
};

#endif  // __ADDPUBLISHERTASK_H_EF4BA438DDB920__

