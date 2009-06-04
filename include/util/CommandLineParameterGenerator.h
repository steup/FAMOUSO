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

#ifndef __CommandLineParameterGenerator_h__
#define __CommandLineParameterGenerator_h__

#include <stdlib.h>
#include <vector>
#include <boost/tokenizer.hpp>
#include "util/CommandLineParameter.h"

#define GENERATE_STRUCT_PARAMETER_HEAD struct Parameter {
#define GENERATE_STRUCT_PARAMETER_PARAM(TYPE, VARIABLE) TYPE VARIABLE;
#define GENERATE_STRUCT_PARAMETER_TAIL };

#define CLP_CLASS_START(class_name, desc, option_name, option_desc, pd1, pd2, pd3, pd4, pd5) \
            class CLP::config::class_name {\
                typedef boost::details::pool::singleton_default<class_name> class_name##__;\
                bool _used;\
                std::vector<std::string> ips;\
                boost::program_options::options_description options;\
              public: \
                class_name() : _used(false), options(desc" Options") {\
                    options.add_options() (option_name,\
                        boost::program_options::value<std::vector<std::string> >(&ips)->composing(),\
                        option_desc); \
                    famouso::util::cmdline_options::instance().add(options);\
                }\
                struct Parameter; \
                static inline class_name& instance() { \
                    return class_name##__::instance();\
                }\
                bool used() {\
                    bool u=_used;\
                    _used=true;\
                    return u;\
                }

#define CLP_CLASS_TRANSFORM_START \
                void getParameter(Parameter& param) {

#define CLP_CLASS_TRANSFORM_MIDDLE \
                    try {\
                        if ( !ips.empty() ) {\
                            used();\
                            typedef boost::tokenizer<boost::char_separator<char> > tokenizer;\
                            boost::char_separator<char> sep(":");\
                            std::string str(ips.back());\
                            ips.pop_back();\
                            tokenizer tokens(str, sep);\
                            tokenizer::iterator tok_iter = tokens.begin(); \
                            try { \

#define CLP_CLASS_TRANSFORM_OPTION(desc, option_type, option_variable) \
                                if (tok_iter!=tokens.end()) {\
                                    param.option_variable=boost::lexical_cast<option_type>(*tok_iter); \
                                    ++tok_iter; \
                                } else { \
                                    throw desc": invalid or incomplete arguments"; \
                                }

#define CLP_CLASS_TRANSFORM_END(desc) \
                             } catch(boost::bad_lexical_cast &) { \
                                 throw desc": invalid or incomplete arguments";\
                             } \
                         } else {\
                            if (used()) \
                                throw desc": to few command line arguments";\
                        }\
                    } catch (const char *c) {\
                        std::cout << c << std::endl << std::endl << options << std::endl;\
                        exit(0);\
                    }\
                }\
           }

#define NAMESPACE(class_name) \
    namespace CLP { \
        namespace config { \
            class class_name; \
        } \
    }

#define CLP1(class_name, desc, option_name, option_desc, pt1, pv1, pd1) \
    NAMESPACE(class_name) \
    CLP_CLASS_START(class_name,desc,option_name,option_desc, pd1, "", "", "", "") \
    GENERATE_STRUCT_PARAMETER_HEAD \
    GENERATE_STRUCT_PARAMETER_PARAM(pt1, pv1) \
    GENERATE_STRUCT_PARAMETER_TAIL \
    CLP_CLASS_TRANSFORM_START \
        param.pv1=pd1;\
    CLP_CLASS_TRANSFORM_MIDDLE \
            CLP_CLASS_TRANSFORM_OPTION(desc, pt1, pv1) \
    CLP_CLASS_TRANSFORM_END(desc) \

#define CLP2(class_name, desc, option_name, option_desc, pt1, pv1, pd1, pt2, pv2, pd2) \
    NAMESPACE(class_name) \
    CLP_CLASS_START(class_name,desc,option_name,option_desc, pd1, pd2, "", "", "") \
    GENERATE_STRUCT_PARAMETER_HEAD \
    GENERATE_STRUCT_PARAMETER_PARAM(pt1, pv1) \
    GENERATE_STRUCT_PARAMETER_PARAM(pt2, pv2) \
    GENERATE_STRUCT_PARAMETER_TAIL \
    CLP_CLASS_TRANSFORM_START \
            param.pv1=pd1;\
            param.pv2=pd2;\
    CLP_CLASS_TRANSFORM_MIDDLE \
            CLP_CLASS_TRANSFORM_OPTION(desc, pt1, pv1) \
            CLP_CLASS_TRANSFORM_OPTION(desc, pt2, pv2) \
    CLP_CLASS_TRANSFORM_END(desc)

#define CLP3(class_name, desc, option_name, option_desc, pt1, pv1, pd1, pt2, pv2, pd2, pt3, pv3, pd3) \
    NAMESPACE(class_name) \
    CLP_CLASS_START(class_name,desc,option_name,option_desc, pd1, pd2, pd3, "", "") \
    GENERATE_STRUCT_PARAMETER_HEAD \
    GENERATE_STRUCT_PARAMETER_PARAM(pt1, pv1) \
    GENERATE_STRUCT_PARAMETER_PARAM(pt2, pv2) \
    GENERATE_STRUCT_PARAMETER_PARAM(pt3, pv3) \
    GENERATE_STRUCT_PARAMETER_TAIL \
    CLP_CLASS_TRANSFORM_START \
            param.pv1=pd1;\
            param.pv2=pd2;\
            param.pv3=pd3;\
    CLP_CLASS_TRANSFORM_MIDDLE \
            CLP_CLASS_TRANSFORM_OPTION(desc, pt1, pv1) \
            CLP_CLASS_TRANSFORM_OPTION(desc, pt2, pv2) \
            CLP_CLASS_TRANSFORM_OPTION(desc, pt3, pv3) \
    CLP_CLASS_TRANSFORM_END(desc)

#define CLP4(class_name, desc, option_name, option_desc, pt1, pv1, pd1, pt2, pv2, pd2, pt3, pv3, pd3, pt4, pv4, pd4) \
    NAMESPACE(class_name) \
    CLP_CLASS_START(class_name,desc,option_name,option_desc, pd1, pd2, pd3, pd4, "") \
    GENERATE_STRUCT_PARAMETER_HEAD \
    GENERATE_STRUCT_PARAMETER_PARAM(pt1, pv1) \
    GENERATE_STRUCT_PARAMETER_PARAM(pt2, pv2) \
    GENERATE_STRUCT_PARAMETER_PARAM(pt3, pv3) \
    GENERATE_STRUCT_PARAMETER_PARAM(pt4, pv4) \
    GENERATE_STRUCT_PARAMETER_TAIL \
    CLP_CLASS_TRANSFORM_START \
            param.pv1=pd1;\
            param.pv2=pd2;\
            param.pv3=pd3;\
            param.pv4=pd4;\
    CLP_CLASS_TRANSFORM_MIDDLE \
            CLP_CLASS_TRANSFORM_OPTION(desc, pt1, pv1) \
            CLP_CLASS_TRANSFORM_OPTION(desc, pt2, pv2) \
            CLP_CLASS_TRANSFORM_OPTION(desc, pt3, pv3) \
            CLP_CLASS_TRANSFORM_OPTION(desc, pt4, pv4) \
    CLP_CLASS_TRANSFORM_END(desc)

#define CLP5(class_name, desc, option_name, option_desc, pt1, pv1, pd1, pt2, pv2, pd2, pt3, pv3, pd3, pt4, pv4, pd4, pt5, pv5, pd5) \
    NAMESPACE(class_name) \
    CLP_CLASS_START(class_name,desc,option_name,option_desc, pd1, pd2, pd3, pd4, pd5) \
    GENERATE_STRUCT_PARAMETER_HEAD \
    GENERATE_STRUCT_PARAMETER_PARAM(pt1, pv1) \
    GENERATE_STRUCT_PARAMETER_PARAM(pt2, pv2) \
    GENERATE_STRUCT_PARAMETER_PARAM(pt3, pv3) \
    GENERATE_STRUCT_PARAMETER_PARAM(pt4, pv4) \
    GENERATE_STRUCT_PARAMETER_PARAM(pt5, pv5) \
    GENERATE_STRUCT_PARAMETER_TAIL \
    CLP_CLASS_TRANSFORM_START \
            param.pv1=pd1;\
            param.pv2=pd2;\
            param.pv3=pd3;\
            param.pv4=pd4;\
            param.pv5=pd5;\
    CLP_CLASS_TRANSFORM_MIDDLE \
            CLP_CLASS_TRANSFORM_OPTION(desc, pt1, pv1) \
            CLP_CLASS_TRANSFORM_OPTION(desc, pt2, pv2) \
            CLP_CLASS_TRANSFORM_OPTION(desc, pt3, pv3) \
            CLP_CLASS_TRANSFORM_OPTION(desc, pt4, pv4) \
            CLP_CLASS_TRANSFORM_OPTION(desc, pt5, pv5) \
    CLP_CLASS_TRANSFORM_END(desc)

#endif

