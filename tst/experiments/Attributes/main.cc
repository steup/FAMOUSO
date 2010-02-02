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

#include "AttributesFramework.h"
#include "EmergencySwitchSpec.h"

#include "Deadline.h"
#include "Period.h"
#include "Omission.h"

// check within a network layer component. This will generate
// a compile time error due to mismatching attributes. The NL
// defines an Omission of 5 and the definition of the Switch
// requires an omission value of 2.
//typedef famouso::mw::Attributes::Properties<>::List<>::CheckAttributeAgainstList<
//                famouso::mw::Attributes::Omission<5>,
//                EmergencySwitchSpec::attributes
//            >::type test;

int main() {

    // define some attributes
    typedef famouso::mw::Attributes::Deadline<50> D50;
    typedef famouso::mw::Attributes::Deadline<30> D30;
    typedef famouso::mw::Attributes::Omission<30> O30;
    typedef famouso::mw::Attributes::Period<30>  P30;

    // define some attribute lists and print their content
    typedef famouso::mw::Attributes::Properties<>::List<D50>::type l1;
    l1::print_elements();

    typedef famouso::mw::Attributes::Properties<>::List<D50, O30>::type l2;
    l2::print_elements();

    typedef famouso::mw::Attributes::Properties<>::List<D30, O30, P30>::type l3;
    l3::print_elements();
    // trying a test between a defined list and an attribute provided by a component
    // that test could be located within a network layer
    typedef famouso::mw::Attributes::Properties<>::List<>::CheckAttributeAgainstList<D30, l3>::type check1;

    // this generates a compile time failure due to duplicated attributes
//    typedef famouso::mw::Attributes::Properties<>::List<D50, O30, P30, D30>::type a4;

    // printing long lists, with repeated attributes is possible by switching the
    // compile time error generation off
    famouso::mw::Attributes::Properties<false>::List<D30, O30, P30, D50>::print_elements();

    // generates compile time failure due to mismatching attributes D50 is not lower or
    // equal than D30, that is part of the list l3
//    typedef famouso::mw::Attributes::Properties<>::List<>::CheckAttributeAgainstList<D50, l3>::type check2;

    // the check can also be done in this way and be printed too
//    std::cout << l3::CheckAttributeAgainstList<D50>::result::value << std::endl;
}

