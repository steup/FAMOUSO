/*******************************************************************************
 *
 * Copyright (c) 2008-2010 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
 * Copyright (c) 2010 Marcus Förster <marcusfoerster1@gmx.de>
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

#ifndef __ExtendedEventChannel_h__
#define __ExtendedEventChannel_h__

#include "mw/common/Subject.h"

#include "boost/mpl/eval_if.hpp"
#include "boost/mpl/vector.hpp"
#include "mw/attributes/AttributeSet.h"
#include "mw/attributes/detail/RequirementChecker.h"
#include "config/type_traits/contains_type.h"

#include "assert/staticwarning.h"

IF_CONTAINS_TYPE_(attributeProvision);

namespace famouso {
    namespace mw {
        namespace api {

            /*!
             * \brief Provides meta functionality for adding an attribute
             *  requirement to a given event channel implementation
             *
             * The given event channel is assumed to define its handler type
             *  which itself can define its appropriate attribute provision.
             *  If no provision is defined, an empty provision is used instead
             *  and a warning is issued.
             *
             * The requirement is expected to be an AttributeSet at the moment.
             *  All attributes contained in that set are checked against an
             *  appropriate attribute of the provision determined as described
             *  above.
             *
             * The checking mechanism could fail either because a required
             *  attribute is not defined in the provision or the attribute values
             *  do not fit. By the compileError flag it is configurable whether
             *  a warning or an error is issued by the compiler.
             *
             * \tparam EC The base event channel
             * \tparam Requirement The attribute requirement as an AttributeSet
             * \tparam compileError True, if an compiler error should be issued
             *  in case of a failure, false for a compiler warning
             */
            template <typename EC, typename Requirement, bool compileError = true>
            class ExtendedEventChannel : public EC {
                public:
                    /*!
                     * \brief Chained event channel constructor, simply calls the
                     *  constructor of the given event channel base class
                     *
                     * \param subject The channel's subject which is simply
                     *  delivered to the base event channel
                     */
                    ExtendedEventChannel(const Subject& subject) : EC(subject) { }

                private:
                    // The event channel handler which is provided by the given
                    //  event channel
                    typedef typename EC::eventChannelHandler ECH;

                    // Find out if the event channel handler defined a provision
                    static const bool echHasProvision =
                            if_contains_type_attributeProvision<ECH>::value;

                    // Issue a warning if it did not
                    FAMOUSO_STATIC_ASSERT_WARNING(
                        echHasProvision,
                        no_provision_defined_by_event_channel_handler,
                        (EC)
                    );

                    // Dummy struct for wrapping an empty provision (in the case that
                    //  the handler did not define a provision)
                    struct EmptyProvisionProvider {
                            typedef attributes::AttributeSet<
                                     boost::mpl::vector<>
                                    > attributeProvision;

                            typedef EmptyProvisionProvider type;
                    };

                    // Attribute provision of the given event channel handler (or an
                    //  an empty provision if the handler did not define any)
                    typedef typename boost::mpl::eval_if_c<
                                      echHasProvision,
                                      ECH,
                                      EmptyProvisionProvider
                                     >::type::attributeProvision echProvision;

                    // The requirement of this event channel given by the attribute set above
                    typedef Requirement ecRequirement;

                    // The type which checks the provision of the ECH against the given
                    //  requirement (This one does the actual work)
                    typedef typename attributes::detail::RequirementChecker<
                                      echProvision,
                                      ecRequirement,
                                      compileError
                                     >::type checker;
            };

        } // namespace api
    } // namespace mw
} // namespace famouso

#endif /* __PublisherEventChannel_h__ */
