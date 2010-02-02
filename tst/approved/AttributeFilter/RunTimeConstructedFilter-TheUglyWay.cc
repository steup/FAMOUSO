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

#include "FilterTestsIncludes.h"


int main(int argc, char **argv) {

    // Runtime filter definition the ugly way
    //
    // different terms of the filter
    // first term has attribute TTL
    typedef filter::UnaryFilterExpression<
                filter::exists<
                    TTL<1>
                >
            > hasTTL;

    // second term is TTL >= 1
    typedef filter::RelationalOperatorFilter<
                filter::exists<
                    TTL<1>
                >,
                IntegralConstRunTimeWrapper<int>,
                filter::greater_than_or_equal_to
            > f1;
    typedef filter::UnaryFilterExpression<
                f1
            > DEf1;
    DEf1    def1(f1(1));

    // third term is TTL >= 1
    typedef filter::RelationalOperatorFilter<
                filter::exists<
                    TTL<1>
                >,
                IntegralConstRunTimeWrapper<int>,
                filter::less_than_or_equal_to
            > f2;
    typedef filter::UnaryFilterExpression<
                f2
            > DEf2;
    DEf2    def2(f2(2));

    // composing the filter
    // first term with an logical_and conjunction
    typedef filter::BinaryFilterExpression<
                hasTTL,
                DEf1,
                filter::logical_and<hasTTL, DEf1>
            > db1;
    typedef filter::UnaryFilterExpression< db1 > DEdb1;
    DEdb1 dedb1(db1(hasTTL((filter::exists<TTL<1> >())), def1));

    // second term with an logical_and conjunction
    typedef filter::BinaryFilterExpression<
                DEdb1,
                DEf2,
                filter::logical_and<DEdb1, DEf2>
            > db2;
    typedef filter::UnaryFilterExpression< db2 > DEdb2;

    // and the filter as object
    DEdb2 dedb2(db2(dedb1, def2));

    // use the filter object
    filter_tester(dedb2);
    return 0;
}
