/*******************************************************************************
 *
 * Copyright (c) 2009-2010 Philipp Werner <philipp.werner@st.ovgu.de>
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


#ifndef __OVERFLOWERRORCHECKING_H_2C2F23FF1575C2__
#define __OVERFLOWERRORCHECKING_H_2C2F23FF1575C2__


namespace famouso {
    namespace mw {
        namespace afp {
            namespace shared {


                /*!
                 * \brief Policy: Overflow error checking
                 *
                 * Use this policy in AFP config to include overflow error checking
                 * code into AFP implementation. This is useful to detect overflows,
                 * underflows and other effects that may occur when AFP is configured
                 * to use too small integer types.
                 *
                 * Alternatives: NoOverflowErrorChecking
                 */
                class OverflowErrorChecking {

                        bool err;

                    public:
                        /// Constructor
                        OverflowErrorChecking() : err(false) {
                        }

                        /// Return whether an error occured
                        bool error() {
                            return err;
                        }


                        /*!
                         * \brief Check if v1 < v2. Else there is an error.
                         */
                        template <class T1, class T2>
                        void check_smaller(T1 v1, T2 v2) {
                            if (!err)
                                err = (v1 >= v2);
                        }

                        /*!
                         * \brief Check if v1 <= v2. Else there is an error.
                         */
                        template <class T1, class T2>
                        void check_smaller_or_equal(T1 v1, T2 v2) {
                            if (!err)
                                err = (v1 > v2);
                        }

                        /*!
                         * \brief Check if v1 == v2. Else there is an error.
                         */
                        template <class T1, class T2>
                        void check_equal(T1 v1, T2 v2) {
                            if (!err)
                                err = (v1 != v2);
                        }

                        /// Make error() returning true
                        void set_error() {
                            err = true;
                        }

                        /// Make error() returning false
                        void reset() {
                            err = false;
                        }
                };


                /*!
                 * \brief Policy: No overflow error checking
                 *
                 * Use this policy in AFP config to include no overflow error checking
                 * code into AFP implementation. This is useful to reduce code size and
                 * execution time, but also reduces robustness of AFP concerning input it
                 * cannot handle (mainly too much data).
                 *
                 * Alternatives: OverflowErrorChecking
                 */
                class NoOverflowErrorChecking {

                    public:
                        /// Constructor
                        NoOverflowErrorChecking() {
                        }

                        /// Return whether an error occured. Always returns false for this policy implementation.
                        bool error() {
                            return false;
                        }


                        /*!
                         * \brief Check if v1 < v2. Does nothing for this policy implementation.
                         */
                        template <class T1, class T2>
                        void check_smaller(T1 v1, T2 v2) {
                        }

                        /*!
                         * \brief Check if v1 <= v2. Does nothing for this policy implementation.
                         */
                        template <class T1, class T2>
                        void check_smaller_or_equal(T1 v1, T2 v2) {
                        }

                        /*!
                         * \brief Check if v1 == v2. Does nothing for this policy implementation.
                         */
                        template <class T1, class T2>
                        void check_equal(T1 v1, T2 v2) {
                        }

                        /// Make error() returning true. Does nothing for this policy implementation.
                        void set_error() {
                        }

                        /// Make error() returning false. Does nothing for this policy implementation.
                        void reset() {
                        }
                };


            } // namespace shared
        } // namespace afp
    } // namespace mw
} // namespace famouso


#endif // __OVERFLOWERRORCHECKING_H_2C2F23FF1575C2__

