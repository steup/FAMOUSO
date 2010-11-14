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

#ifndef __famouso_h__
#define __famouso_h__

#include "mw/el/EventChannelHandler.h"
#include "util/CommandLineParameter.h"

#include "config/type_traits/if_contains_type.h"
#include "config/policies/StaticCreatorPolicy.h"


namespace famouso {

    /*!\brief The namespace configuration contains all additional generated
     *        tester and selector classes that are needed for the automatic
     *        %configuration process. It is used for storing all relevant
     *        entities in one place to bar namespace clashes.
     */
    namespace configuration {
        /*! \brief This macro generates a compile time tester and selector class
         *         for checking if ELMS (famouso::mw::el::EventLayerMiddlewareStub)
         *         is a subtype of a class/%configuration. This feasibility is used
         *         for %configuration decisions during the %configuration process of
         *         the middleware.
         */
        IF_CONTAINS_TYPE_(ELMS);

        /*! \brief This macro generates a compile time tester and selector
         *         class for checking if GW (famouso::mw::gwl::Gateway) is a
         *         subtype of a class/%configuration. This feasibility is used
         *         for %configuration decisions during the %configuration process
         *         of the middleware.
         */
        IF_CONTAINS_TYPE_(GW);
    }

    /*! \brief  The init function of %FAMOUSO initialize the Event Channel
     *          Handler, its sub-objects and let all %configuration protocols
     *          run.
     *
     *  \tparam T contains a %FAMOUSO %configuration description.
     */
    template <class T>
    inline void init() {
        famouso::mw::el::EventChannelHandler<typename T::EL>::create();
        famouso::mw::el::EventChannelHandler<typename T::EL>::ech().init<typename T::EL>();

        // dependent initialization, if the event layer middleware stub is configured
        configuration::if_contains_type_ELMS<T>::template ThenElse<StaticCreatorPolicy>::process();

        // dependent initialization, if a gatway is configured
        /*! \todo allow initialization of a GW only in case of more then one
         *        network layer available. This can be determind with the help
         *        of a configured NetworkAdapter. Is this type is missing in
         *        general non Gateway may exist.
         */
        configuration::if_contains_type_GW<T>::template ThenElse<StaticCreatorPolicy>::process();
    }

    /*! \brief  The init function of %FAMOUSO with support for command line parameter
     *          detection and evaluation. After that, it calls the standard famouso::init()
     *          function to initialize the middleware as usual.
     *
     *  \tparam T contains a %FAMOUSO %configuration description.
     */
    template <class T>
    inline void init(int argc, char** argv) {
        // first, parse command line parameter before any famouso middleware
        // component will be initalized.
        famouso::util::clp(argc,argv);
        init<T>();
    }
}

#endif
