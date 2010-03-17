/*******************************************************************************
 *
 * Copyright (c) 2008-2010 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
 *               2009-2010 Michael Kriese <kriese@cs.uni-magdeburg.de>
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
#ifndef __lists_hpp__
#define __lists_hpp__

#include <list>
#include "boost/shared_ptr.hpp"
#include "boost/noncopyable.hpp"

namespace famouso {
    namespace mw {
        namespace nl {
            namespace awds {

                template< class Type >
                class List: boost::noncopyable {
                    private:
                        typedef std::list<Type> list;

                    public:
                        typedef boost::shared_ptr<List<Type> > type;
                        typedef typename list::iterator iterator;
                        typedef typename list::size_type size_type;

                        static type Create() {
                            type res = type(new List<Type>());
                            return res;
                        }

                        iterator begin() {
                            return _list.begin();
                        }

                        iterator end() {
                            return _list.end();
                        }

                        size_type size() const {
                            return _list.size();
                        }

                        void add(Type c) {
                            _list.push_back(c);
                        }

                        void remove(Type c) {
                            _list.remove(c);
                        }

                        iterator erase(iterator it) {
                            return _list.erase(it);
                        }

                        void clear() {
                            _list.clear();
                        }

                    private:
                        list _list;
                };

            } /* awds */
        } /* nl */
    } /* mw */
} /* famouso */
#endif /* __lists_hpp__ */
