/*
 * AWDS, a wrapper to communicate with the AWDS
 * Copyright (C) 2006 Dipl. Inform. Michael Schulze, Dipl. Inform Andre` Herms
 * Embedded Systems and Operating Systems
 * Department of Distributed Systems (IVS)
 * Faculty of Computer Science
 * Otto-von-Guericke-University Magdeburg, Germany
 *
 * mschulze@ivs.cs.uni-magdeburg.de
 * aherms@ivs.cs.uni-magdeburg.de
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA.
 */

/*$Id: awds.h 204 2008-02-14 12:38:29Z lindhors $*/


/*!
 * \file awds.h
 * \brief AWDS specific connector
 *
 */


#ifndef AWDS_H_
#define AWDS_H_



#include <stdint.h>
#include "anl/st/st_message.h"

// #ifndef __cplusplus
// #error  This is a C++ header, it should never be included by pure C code.
// #endif

#ifdef __cplusplus
extern "C" {
#endif

    /*!
     * \brief AWDS send routine.
     *
     * \param[in] cmd CAN-command, as specified by can.h
     *
     * \return 0 In case of success,\n
     *		   1 otherwise
     *
     * \note Inlined routine.
     */
    uint8_t awds_send(st_message_t* st_msg );

    /*
     * \note Everything a little bit crappy/hack-y due to lack of time :(
     */
    uint8_t awds_init();


    uint8_t awds_bind_announce(subject_t* subject);

    uint8_t awds_bind_subscribe(subject_t* subject);

    void    awds_unbind_announce(subject_t* subject);

    void    awds_unbind_subscribe(subject_t* subject);




#ifdef __cplusplus
}
#endif



#endif /*AWDS_H_*/

/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */
