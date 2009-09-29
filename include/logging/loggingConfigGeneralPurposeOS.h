/*******************************************************************************
 *
 * Copyright (c) 2008, 2009 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
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

#ifndef __loggingConfigGeneralPurposeOS_h__
#define __loggingConfigGeneralPurposeOS_h__

#include "logging/OutputLevelRunTimeSwitch.h"
#include "logging/OutputLevelSwitchDisabled.h"
#include "logging/OutputStream.h"

#include "logging/StdOutput.h"
#include "logging/FileOutput.h"

namespace logging {
    /*!\brief The %_config_ namespace separates the configurations of the %logging
     *        framework from the %logging namespace avoiding name clashes.
     *
     *        It contains different possible configurations for different
     *        platforms, but the configurations are not limited to the yet
     *        provided one.
     */
    namespace _config_ {

        /*!\brief FileLogType is a possible %logging type, supporting %logging
         *        to a file on general purpose operating systems
         */
        typedef ::logging::OutputLevelSwitchDisabled <
                    ::logging::OutputStream <
                        ::logging::OutputBaseType<
                            ::logging::FileOutput
                        >
                    >
                > FileLogType;

        /*!\brief StdLogType is a possible %logging type, supporting %logging
         *        to standard output on general purpose operating systems
         */
        typedef ::logging::OutputLevelSwitchDisabled <
                    ::logging::OutputStream <
                        ::logging::OutputBaseType<
                            ::logging::StdOutput
                        >
                    >
                > StdLogType;

        /*!\brief StdLogRunTimeSwitchType is a possible %logging type,
         *        supporting %logging to standard output on general purpose
         *        operating systems with the additional feature of switching
         *        the verbosity of the %logging framework at runtime.
         */
        typedef ::logging::OutputLevelRunTimeSwitch <
                    ::logging::OutputStream <
                        ::logging::OutputBaseType<
                            ::logging::StdOutput
                        >
                    >
                > StdLogRunTimeSwitchType;
    }

    /*!\brief base logging type for the respective platform
     */
    typedef ::logging::_config_::StdLogType LoggingType;
}


#endif