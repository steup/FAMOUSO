/* 
 * AT90CAN, a CAN driver for the AT90-series of Atmel 
 * Copyright (C) 2006 Joerg Diederich, Dipl. Inform. Michael Schulze 
 * Embedded Systems and Operating Systems 
 * Department of Distributed Systems (IVS)
 * Faculty of Computer Science
 * Otto-von-Guericke-University Magdeburg, Germany
 *
 * joerg.diederich@graffiti.net
 * mschulze@ivs.cs.uni-magdeburg.de 
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

/*$Id$*/


/*!
 * \file at90can.c
 * \brief AT90 specific CAN driver
 *
 * Definitions of a CAN driver for the AT90CAN microcontroller. 
 */


#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "devices/nic/can/at90can/at90can.h"
//#include "cosmic/cosmiccfg.h"

#ifdef USE_IRQ_REDIRECTION
#include "redirvec.h"
#endif /* USE_IRQ_REDIRECTION */
  
  
// global variables  
#ifdef CONF_CAN_USE_RX_IRQ

/*!
 * Receive handler.
 */
can_handler rx_handler;

/*!
 * Mode of buffer management.
 */
enum CAN_IRQ_RX_MODE irq_rx_mode;

/*!
 * last treated message object number
 */
MOb irq_rx_mob;
  
#endif /*CONF_CAN_USE_RX_IRQ*/
  
  
uint8_t
can_init(enum CAN_CTRL_MODE mode){
    
  // this is more board stuff .. maybe better there next time
  DDRD |= 0x80;
  PORTD = 0;

  can_reset();
  can_set_bitrate();
  if ( can_init_mobs(mode)){
  	return 1;
  }
  else{
	can_enable();
	can_await_control_rdy();
#ifdef CONF_CAN_USE_RX_IRQ
	rx_handler = NULL;
	irq_rx_mob = 0;
#ifdef USE_IRQ_REDIRECTION	
	set_int_handler(&can_irq_handler, INT_CANIT);
#endif /* USE_IRQ_REDIRECTION */	
#endif /* CONF_CAN_USE_RX_IRQ */
	return 0;
  }
}

uint8_t
can_cmd(can_cmd_t* cmd)
{
	if ( cmd ){
		switch ( cmd->cmd ){
			case CAN_CMD_NONE:
				return 0;
				break;
			case CAN_CMD_TX_DATA:
				return canary_send(cmd->msg, can_get_free_mob(0));
				break;
#ifdef CONF_CAN_USE_RX_IRQ
			case CAN_CMD_IRQ_RX_DATA:
				return canary_recv(cmd->msg, irq_rx_mob);
				break;
#endif /* CONF_CAN_USE_RX_IRQ */
			default:
				break;
		}
	}	
	return 1;
}


uint8_t 
canary_send(can_msg_t* msg, MOb mob)
{
  	uint8_t save = CANPAGE;

	if ( ! can_select_mob(mob)){
	  	if ( msg ){
  			// set msg
			can_mob_set_msg(msg);
  			// set transmit
  			can_mob_enable_tx();
  			// why poll? in order to be ready for send further messages
  			// CAUTION: if interrupted by IRQ, and IRQ wants to send,
  			can_mob_await_tx_rdy();
  			can_mob_clear_status();
  			can_mob_clear_ctrl();
	  		return 0;
  		}
	  	else{
			DEBUG("CAN: send, missing message\n");  			
  		}
  		CANPAGE = save;
  	}
  	return 1;
}

uint8_t
canary_recv(can_msg_t* msg, MOb mob)
{
  	uint8_t save = CANPAGE;
//char sf[10];
	if ( ! can_select_mob(mob)){
		can_mob_get_msg(msg);
//sprintf(sf,"rx id 0x%02x\n",msg->ident.id.tab[1]);
//DEBUG(sf);
	}
	CANPAGE = save;
	return 0;
}
			
MOb
can_get_conf_mob(uint8_t conf, uint8_t ignore)
{
    uint8_t save;

    save = CANPAGE;
    while ( ! can_select_mob(ignore) ){
        if ( (can_mob_get_ctrl() & ((1<<CONMOB1) | (1<<CONMOB0)) ) == conf){
        	// MOb with desired configuration found 
			break;
        }
        ignore++;
    }
   	// restore previous status and exit
   	CANPAGE = save;
   	return ignore;
}





/*
 * 
 * 
 * 
 * IRQ related routines.
 * 
 * 
 * 
 */

#ifdef CONF_CAN_USE_IRQ


void can_notify_handler()
{
	// look for a mob
	if ( CANSTMOB & (1<<TXOK )){
    	// TODO: call transmit handler
  	}

#ifdef CONF_CAN_USE_RX_IRQ
    if ( CANSTMOB & (1<<RXOK) ){
    	// call rx callers
		switch(irq_rx_mode){
		 	case CAN_IRQ_RX_AUTO_STDY:
		 		// mob pointer still valid
		 		break;
			case CAN_IRQ_RX_AUTO_INC:
				// increase last accessed mob
				irq_rx_mob++;
				if ( irq_rx_mob == CANARY_MOB_NUM ){
					// wrap around
					irq_rx_mob = 0;
				}
				break;
		 	default:
		 		break;
		} 
		irq_rx_mob = can_get_irq_mob(irq_rx_mob, CANARY_MOB_STATUS_RECEIVE_OK);
		// call receive handler
   		if ( rx_handler ){
      		(*rx_handler)();
   		}
   		// clear rx flag
		if ( ! can_select_mob(irq_rx_mob)){
      		can_mob_reset_rx();
		}
    }
#endif /* CONF_CAN_USE_RX_IRQ */

    if ( CANSTMOB & ( (1<<BERR )|(1<<SERR)|(1<<CERR)|(1<<FERR )
		      |(1<<AERR) )){
      // call error handler
    }
}

/*!
 * \brief Interrupt handler for CAN controller irq.
 *
 * Interrupt handler routine of CAN controller interrupt.
 *
 * \todo: enable general irq handling
 * \todo check highest priority functionality
 */
#ifndef USE_IRQ_REDIRECTION	
ISR(CANIT_vect)
#else
void
can_irq_handler()
#endif /* USE_IRQ_REDIRECTION */	
{
	uint8_t canpage_save;

  	// check for irq source or mob irq
  	if ( CANGIT & ( (1 << BOFFIT)|(1 << BXOK)|(1 << SERG)|(1 << CERG)
		  	|(1 << FERG)|(1 << AERG) ) ){ 
    	/*
      	ignore CAN timer overrun irq's! Bit 5 is set whenever an overrun of 
      	CAN timer occurs. BUT irq generation is independend from that. The
      	CAN timer overrun has it's own irq vector OVRIT_vect. The CANIT_vect
      	vector is not called by this, even if CANGIT has a raised OVRTIM flag.
    	*/
    	// general irq. disable flag
    	CANGIT = 0x5F;
  	}
  	else{
		canpage_save = CANPAGE;
		/*
		 *  multiple irq's from various reasons are possibly active at this 
		 * point. so what to do?
		 * 
		 * decision: use highest priority first, and later use specified mode 
		 * (buffer management)
		 */
		if ( ! can_select_mob(can_get_irq_mob_highest())){ 
			// call appropriate handler
			can_notify_handler();
		}
    	CANPAGE = canpage_save;
  	}
}


#ifdef CONF_CAN_USE_RX_IRQ

uint8_t
can_set_rx_handler(can_handler handler, enum CAN_IRQ_RX_MODE mode)
{
	uint8_t save;
	MOb mob;

  	if ( handler ){
	    // set function pointer. disable generation of rx irq's first
	    can_disable_rx_irq();
		rx_handler = handler;
		// set mode
		irq_rx_mode = mode;
		save = CANPAGE;
		// enable irq generation for all receiving mobs
		mob = can_get_recv_mob(0);
	    while ( ! can_select_mob(mob) ){
	    	// enable irq for mob
	    	can_enable_mob_irq(mob);
	    	mob = can_get_recv_mob(++mob);
	    }
		CANPAGE = save;
   		// enable CAN recv interrupts
   		can_enable_rx_irq();
   		return 0;
  	}
  	return 1;
}

void 
can_remove_rx_handler()
{
	uint8_t save;
	MOb mob;

	// disable CAN recv interrupts
    can_disable_rx_irq();
	rx_handler = NULL;
	// reset mode
	irq_rx_mode = CAN_IRQ_RX_NONE;
	save = CANPAGE;
	// disable irq generation for all receiving mobs
	mob = can_get_recv_mob(0);
    while ( ! can_select_mob(mob) ){
    	// disable irq for mob
		can_disable_mob_irq(mob);
    	mob = can_get_recv_mob(++mob);
    }
	CANPAGE = save;
    return;
}

#endif /* CONF_CAN_USE_RX_IRQ */

#endif /* CONF_CAN_USE_IRQ */




