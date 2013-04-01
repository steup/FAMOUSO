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

#ifndef __CommandLineParameterProvider_h__
#define __CommandLineParameterProvider_h__

#include <boost/thread/detail/singleton.hpp>
#include <boost/program_options.hpp>

namespace famouso {

    namespace util {

        /*! \brief This is option_description type.
         */
        typedef boost::program_options::options_description CommandLineParameter_type;

        /*! \brief The cmdline_option is a singleton providing
         *         the interface to the command line parameter
         *         framework. It is used to put new options into
         *         the general description enabling to add options
         *         from different components in a coherent way.
         *         All components use this to summarize the options
         *         in one place
         */
        typedef boost::detail::thread::singleton<CommandLineParameter_type> cmdline_options;


        /*! \brief This type describes the map, where the command
         *         line parameter are parsed into.
         */
        typedef boost::program_options::variables_map vm_type;

        /*! \brief The vm is the interface to the parsed command line
         *         parameters. It is realized as a singleton allowing access
         *         from different places/components always to the same instance
         *         of the parsed parameter.
         */
        typedef boost::detail::thread::singleton<vm_type> vm;

    }
}

#endif
