/*******************************************************************************
 *
 * Copyright (c) 2008-2010 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
 *                    2010 Marcus Foerster <MarcusFoerster1@gmx.de>
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

#ifndef _Attribute_Set_Impl_
#define _Attribute_Set_Impl_

#include <stdint.h>

#include "boost/mpl/begin.hpp"
#include "boost/mpl/end.hpp"
#include "boost/mpl/size.hpp"
#include "boost/mpl/deref.hpp"
#include "boost/mpl/next.hpp"

#include "mw/attributes/detail/AttributeSize.h"

#include "object/PlacementNew.h"

namespace famouso {
    namespace mw {
        namespace attributes {
            namespace detail {

                template <typename AttrSeq,
                          typename Iter = typename boost::mpl::begin<AttrSeq>::type>
                struct AttributeSetImpl {
                    private:
                        // The current attribute type (determined by the given iterator type)
                        typedef typename boost::mpl::deref<Iter>::type curAttr;

                        // The next sequence element type
                        typedef typename boost::mpl::next<Iter>::type iterNext;

                    public:
                        // This type
                        typedef AttributeSetImpl type;

                        // The size in bytes of this attribute set
                        static const uint16_t size = AttributeSize<curAttr>::value +
                                                     AttributeSetImpl<AttrSeq, iterNext>::size;

                        // The number of attributes contained in this set
                        static const uint16_t count = boost::mpl::size<AttrSeq>::value;

                        static void construct(uint8_t *pos){
                            // Construct the attribute currently pointed to by the
                            //  given iterator
                            new (pos) curAttr;
                            // Then construct the rest of the set recursively
                            AttributeSetImpl<AttrSeq, iterNext>::construct(pos+AttributeSize<curAttr>::value);
                        }

                    private:
                        AttributeSetImpl();
                        AttributeSetImpl(const AttributeSetImpl&);
                        const AttributeSetImpl& operator=(const AttributeSetImpl&);
                };

                template <typename AttrSeq>
                struct AttributeSetImpl<AttrSeq, typename boost::mpl::end<AttrSeq>::type> {
                        // This type
                        typedef AttributeSetImpl type;

                        static const uint16_t size = 0;

                        static const uint16_t count = 0;

                        static void construct(uint8_t *){}

                    private:
                        AttributeSetImpl();
                        AttributeSetImpl(const AttributeSetImpl&);
                        const AttributeSetImpl& operator=(const AttributeSetImpl&);
                };

            } // end namespace detail
        } // end namespace attributes
    } // end namespace mw
} // end namespace famouso

#endif // _Attribute_Set_Header_
