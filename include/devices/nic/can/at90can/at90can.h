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
 * \file at90can.h
 * \brief MCU specific CAN driver
 * 
 * CAN driver for the AT90CAN128 microcontroller. Supports
 * redirection, if specified. 
 *
 * \note It's just intended (and tested) for the mcu spoken of. It should 
 * be possible to use it for other AVR's too, as they provide the same 
 * interface.
 */


#ifndef __CAN_DRV_CAN128_H
#define __CAN_DRV_CAN128_H

#include <stddef.h>
#include <stdint.h>
#include <avr/io.h>

//#include "nic/can.h"
//#include "cosmic/cosmiccfg.h"

#ifndef F_CPU
#error  You have to define a clock frequency in "cosmiccfg.h"
#endif
#ifndef CONF_CAN_BITRATE
#error  You have to define a bitrate for the CAN in "cosmiccfg.h"
#endif


#if 0
#define DEBUG(string);							\
	{								\
		char* stri = string;					\
		while( *stri ){						\
        	/* Wait for empty transmit buffer */			\
        	while ( !(UCSR0A & (1<<UDRE0)) );                       \
        	/* Start transmittion */				\
			UDR0 = (unsigned char) *stri++;			\
		}							\
	}
#else
#define	DEBUG(string);																	
#endif


/*
 * Macro substitution for CAN bitrate.
 *
 * \todo Enable bitrates for other clock frequencies and time quantums (TQ)
 */
#if F_CPU == 16000000
#if CONF_CAN_BITRATE == 100     
#define CONF_CANBT1  0x12       
#define CONF_CANBT2  0x0C      
#define CONF_CANBT3  0x37 
#elif CONF_CAN_BITRATE == 125   
#define CONF_CANBT1  0x0E     
#define CONF_CANBT2  0x0C     
#define CONF_CANBT3  0x37
#elif CONF_CAN_BITRATE == 200     
#define CONF_CANBT1  0x08  
#define CONF_CANBT2  0x0C  
#define CONF_CANBT3  0x37
#elif CONF_CAN_BITRATE == 250    
#define CONF_CANBT1  0x06
#define CONF_CANBT2  0x0C
#define CONF_CANBT3  0x37
#elif CONF_CAN_BITRATE == 500    
#define CONF_CANBT1  0x06
#define CONF_CANBT2  0x04
#define CONF_CANBT3  0x13
#elif CONF_CAN_BITRATE == 1000   
#define CONF_CANBT1  0x02
#define CONF_CANBT2  0x04
#define CONF_CANBT3  0x13
#else
#error Selected CONF_CAN_BITRATE value is unknown.
#endif /* CONF_CAN_BITRATE */
#else
#error Selected F_CPU value is unknown.
#endif /* F_CPU  */


/*!
 * \brief CAN data mode marker autoinc
 * 
 * Marks autoincrement mode for data handling, which is used in filling a mob
 * with data.
 */
#define CANARY_DATA_MODE_AUTOINC 0

/*!
 * \brief MCU constants.
 *
 * MCU specific properties.
 */
#define CANARY_MOB_NUM 15 /*!< Number of available message objects. */


/*!
 * Configuration of MOb
 */
#define CANARY_MOB_CONF_DISABLED 0x00
#define CANARY_MOB_CONF_ENABLED_RECEPTION (1<<CONMOB1)

/*!
 * Status of MOb
 */
#define CANARY_MOB_STATUS_RECEIVE_OK (1 << RXOK)

/*!
 * Message object type.
 */
typedef uint8_t MOb;





/*
 * 
 * 
 *  MOb related routines
 * 
 * 
 */														  

/*!
 * \brief Set MOb status.
 *
 * Set status of actual message object.
 */
inline void can_mob_set_status(uint8_t val){
  CANSTMOB = val;
}

/*!
 * \brief Get MOb status.
 *
 * Get status of actual message object.
 */
inline uint8_t can_mob_get_status(){
  return CANSTMOB;
}

/*!
 * \brief Clear MOb status.
 *
 * Clears status of actual message object.
 * 
 *\note Requires a read-modify-write in case of irq ACK, see manual page 248. 
 * Ensure the used library implementation performs this (avr-libc does by using 
 * volatile pointers).
 */
inline void can_mob_clear_status(){
	can_mob_set_status(0);	
}

/*!
 * \brief Set MOb ctrl.
 *
 * Set control register of actual message object.
 */
inline void can_mob_set_ctrl(uint8_t val){
  CANCDMOB = val;
}

/*!
 * \brief Get MOb ctrl.
 *
 * Get control bits of actual message object.
 */
inline uint8_t can_mob_get_ctrl(){
  return CANCDMOB;
}

/*!
 * \brief Clear MOb ctrl.
 *
 * Clear control register of actual message object.
 */
inline void can_mob_clear_ctrl(){
	can_mob_set_ctrl(0);	
}

/*!
 * \brief Set standard CAN identifier.
 *
 * Set standard CAN identifier. 
 *
 * \param[in] id Pointer to identifier union.
 */
inline void can_mob_set_std_id(uint16_t id){
	id <<= 5;
	CANIDT2 = (uint8_t) (id & 0xFF);
	CANIDT1 = (uint8_t) id >> 8;
	can_mob_set_ctrl( can_mob_get_ctrl() & ~(1<<IDE));
}

/*!
 * \brief Set extended CAN identifier.
 *
 * Set extended CAN identifier. 
 *
 * \param[in] id Pointer to identifier union.
 */
inline void can_mob_set_ext_id( uint32_t id){
	id <<= 3;
	CANIDT4 = (uint8_t) (id & 0xFF);
	CANIDT3 = (uint8_t) (id >> 8);
	CANIDT2 = (uint8_t) (id >> 16);
	CANIDT1 = (uint8_t) (id >> 24);
	can_mob_set_ctrl( can_mob_get_ctrl() | (1<<IDE));
}

/*!
 * \brief Get standard CAN identifier.
 *
 * Get standard CAN identifier. 
 *
 * \param[out] id Pointer to identifier union.
 */
inline void can_mob_get_std_id(can_id_t* id){
    id->tab[1] = CANIDT1;
    id->tab[0] = CANIDT2;
    id->std >>= 5;
}

/*!
 * \brief Get extended CAN identifier.
 *
 * Get extended CAN identifier. 
 *
 * \param[out] id Pointer to identifier union.
 */
inline void can_mob_get_ext_id(can_id_t* id){
    // works with little endian. will kick unused bits out of tab[0]
	id->tab[3] = CANIDT1;
    id->tab[2] = CANIDT2;
    id->tab[1] = CANIDT3;
    id->tab[0] = CANIDT4;
    id->ext >>= 3; 
}

/*!
 * \brief Check for extended identifier.
 *
 * Checks, if extended identifier has to be used.
 *
 * \return True If extended identifiers has to be used.\n
 * False If standard identifiers are sufficient.
 */
inline uint8_t can_mob_has_ext_id(){
  return ( can_mob_get_ctrl() & (1 << IDE));
}

/*!
 * \brief Set CAN identifier.
 *
 * Set CAN identifier. 
 *
 * \param[in] id Pointer to identifier union.
 * \param[in] extended Use of extended identifier.
 */
inline void can_mob_set_id( can_identifier_t* id){
	/* 
       pay attention: little endian! so it starts with the least 
       significant byte. that's why index 0 is for the lowest id bits,
       index 1 for the second lowest and so on.
    */
  	if ( id->type == CAN_ID_EXTENDED){
  		can_mob_set_ext_id(id->id.ext);
  	}
  	else{
  		can_mob_set_std_id(id->id.std);
  	}
}

/*!
 * \brief Get CAN identifier.
 *
 * Get CAN identifier. 
 *
 * \param[out] id Pointer to identifier union.
 */
inline void can_mob_get_id(can_identifier_t* id){
  if ( can_mob_has_ext_id()){
  	can_mob_get_ext_id(&id->id);
  	id->type = CAN_ID_EXTENDED;
  }
  else{
  	can_mob_get_std_id(&id->id);
  	id->type = CAN_ID_STANDARD;
  }
}

/*!
 * \brief Enable extended mask.
 *
 * Enable masking for extended identifiers by setting appropriate bits.
 */
inline void can_mob_enable_ext_msk(){
  CANIDM4 |= (1<<IDEMSK);
}

/*!
 * \brief Enable standard mask.
 *
 * Enable masking for standard identifiers by setting appropriate bits.
 */
inline void can_mob_enable_std_msk(){
  CANIDM4 &= ~(1<<IDEMSK);
}


/*!
 * \brief Set masking for std identifier.
 *
 * Set masking for standard identifier.
 *
 * \param[in] id Pointer to identifier union.
 */
inline void can_mob_set_std_msk(uint16_t id){
    id <<= 5;
    CANIDM2 = (uint8_t) (id & 0xFF);
    CANIDM1 = (uint8_t) (id >> 8);
    can_mob_enable_std_msk();
}

/*!
 * \brief Set masking for ext identifier.
 *
 * Set masking for extended identifier.
 *
 * \param[in] id Pointer to identifier union.
 */
inline void can_mob_set_ext_msk(uint32_t id){
    id <<= 3;
    CANIDM4 = (uint8_t) (id & 0xFF);
    CANIDM3 = (uint8_t) (id >> 8);
    CANIDM2 = (uint8_t) (id >> 16);
    CANIDM1 = (uint8_t) (id >> 24);
	can_mob_enable_ext_msk();    
}

/*!
 * \brief Set masking for identifier.
 *
 * Set masking for identifier.
 *
 * \param[in] id Pointer to identifier union.
 */
inline void can_mob_set_msk(can_identifier_t* id){
  if ( id->type == CAN_ID_EXTENDED ){
  	can_mob_set_ext_msk(id->id.ext);
  }
  else{
  	can_mob_set_std_msk(id->id.std);
  }
}

/*!
 * \brief Enable transmission of MOb
 *
 * Enable transmission by setting appropriate bits in message object register.
 */
inline void can_mob_enable_tx() {
  can_mob_set_ctrl( can_mob_get_ctrl() | (1<<CONMOB0));
  can_mob_set_ctrl( can_mob_get_ctrl() & ~(1<<CONMOB1));
} 

/*!
 * \brief Disable transmission of MOb
 *
 * Disable transmission by setting appropriate bits in message object register.
 */
inline void can_mob_disable_tx() {
  can_mob_set_ctrl( can_mob_get_ctrl() & ~(1<<CONMOB0));
} 

/*!
 * \brief Clear MOb status of tx.
 *
 * Clear transmission status of actual message object.
 * 
 * \note Requires a read-modify-write in case of irq ACK, see manual page 248.
 */
inline void can_mob_clear_status_tx(){
  can_mob_set_status( can_mob_get_status() & ~(1<<TXOK));
}

/*!
 * \brief Reset tx.
 *
 * Performs resetting of a MOb for transmission.
 */
inline void can_mob_reset_tx(){
	// clear status register flag
  	can_mob_clear_status_tx();
  	// prepares control register for more transmissions
  	can_mob_enable_tx();
}

/*!
 * \brief Enable recpetion of MOb
 *
 * Enable recepetion by setting appropriate bits in message object register.
 */
inline void can_mob_enable_rx(){
  can_mob_set_ctrl( can_mob_get_ctrl() | (1<<CONMOB1));
}

/*!
 * \brief Disable reception of MOb
 *
 * Disable reception by setting appropriate bits in message object register.
 */
inline void can_mob_disable_rx(){
  can_mob_set_ctrl( can_mob_get_ctrl() & ~(1<<CONMOB1));
}

/*!
 * \brief Clear MOb status of rx.
 *
 * Clear reception status of actual message object.
 * 
 * \note Requires a read-modify-write in case of irq ACK, see manual page 248.
 */
inline void can_mob_clear_status_rx(){
  can_mob_set_status( can_mob_get_status() & ~(1<<RXOK));
}

/*!
 * \brief Reset rx.
 *
 * Performs resetting of a MOb for reception.
 */
inline void can_mob_reset_rx(){
	// clear status register
	can_mob_clear_status_rx();
	// reset control register
  	can_mob_enable_rx();
}

/*!
 * \brief Set dlc.
 *
 * Set data length code by setting appropriate bits.
 *
 * \param[in] dlc Data length code to set.
 */
inline void can_mob_set_dlc(uint8_t dlc){
	// erase dlc bits first.
  	can_mob_set_ctrl( can_mob_get_ctrl() & 0xF0);
  	// set dlc bits 
  	can_mob_set_ctrl( can_mob_get_ctrl() | ( dlc & 0x0F ));
}

/*!
 * \brief Get dlc.
 *
 * Get data length code by reading appropriate bits.
 *
 * \return Data length code set.
 */
inline uint8_t can_mob_get_dlc(){
  return (can_mob_get_ctrl() & 0x0F);
}

/*!
 * \brief Wait for MOb tx rdy.
 *
 * Waits until message object is ready for further transmissions.
 */
inline void can_mob_await_tx_rdy(){
	while (!(can_mob_get_status() & (1<<TXOK))){
  	}
}

/*!
 * \brief Set data of MOb.
 *
 * Set data of message object. 
 *
 * \param[in] data Pointer to data.
 * \param[in] mode Mode of MOb data operation.
 */
inline void can_mob_set_data(can_data_t* data, uint8_t mode){
	
	uint8_t len = data->len;
	
	// correct len to maximal value	
	if ( len > CAN_MAX_PAYLOAD ){
		len = CAN_MAX_PAYLOAD;
	}									
	if ( mode == CANARY_DATA_MODE_AUTOINC ){
		// loop over all bytes and fill with values
		uint8_t i;
		
		for ( i = 0; i < len; i++){
	      	CANMSG = data->data.tab[i];
		}
	}
	can_mob_set_dlc(len);
	
	// TODO: else case, NO autoinc
}

/*!
 * \brief Get data of MOb.
 *
 * Get data of message object. 
 *
 * \param[out] data Pointer to data.
 * \param[in] len Length of data. Will be corrected, if it exceeds CAN limit. 
 * \param[in] mode Mode of MOb data operation.
 * 
 */
inline void can_mob_get_data(can_data_t* data, uint8_t mode)
{
	uint8_t i;
	
	data->len = can_mob_get_dlc();

	if ( mode == CANARY_DATA_MODE_AUTOINC ){
		for ( i = 0; i < data->len; i++){
    		data->data.tab[i] = CANMSG;
  		}
	}

	// TODO: else case, NO autoinc
}

/*!
 * \brief Set message in MOb.
 *
 * Set CAN message in message object.
 * 
 * \param[in] msg CAN message to store into message object.  
 */ 
inline void can_mob_set_msg(can_msg_t* msg)
{
	can_mob_set_id(&msg->ident);
	can_mob_set_data(&msg->data, CANARY_DATA_MODE_AUTOINC);	
}

/*!
 * \brief Get message of MOb.
 *
 * Get CAN message from message object.
 * 
 * \param[out] msg CAN message to store into message object.  
 */ 
inline void can_mob_get_msg(can_msg_t* msg)
{
	can_mob_get_id(&msg->ident);
	can_mob_get_data(&msg->data, CANARY_DATA_MODE_AUTOINC);
}

/*
 * \brief Init a MOb.
 *
 * Initialze a message object. Includes data, status, identifier and mask
 * registers.
 */
inline void can_mob_init(){

	can_identifier_t zero_ext_ident = {{0},CAN_ID_EXTENDED};
	can_data_t zero_data = {{0},CAN_MAX_PAYLOAD};

	// clear control
	can_mob_clear_ctrl();
	// clear status
	can_mob_clear_status();
	// clear identifier
	can_mob_set_id(&zero_ext_ident);
	// clear identifier mask
	can_mob_set_msk(&zero_ext_ident);
	// clear data in autoincrement mode (default)
	can_mob_set_data(&zero_data, CANARY_DATA_MODE_AUTOINC);
}



/*
 * 
 * 
 * 
 * Global CAN controller routines.
 * 
 * 
 * 
 * 
 */

/*!
 * \brief Send data.
 *
 * Sends data via CAN according to the settings given in the argument.
 *
 * \param[in] cmd Pointer to CAN message. Contains further details
 * like identifier, dlc and so on.
 * \param[in] mob Message object to use.
 *
 * \return 0 In case of success.\n
 * 		   1 Otherwise.
 */
uint8_t canary_send(can_msg_t* msg, MOb mob);

/*!
 * \brief Receive data.
 *
 * Receive data via CAN. 
 *
 * \param[in] cmd Pointer to CAN message. Contains further details
 * like identifier, dlc and so on.
 * \param[in] mob Message object to use.
 *
 * \return 0 in any case.
 */
uint8_t canary_recv(can_msg_t* msg, MOb mob);

/*!
 * \brief Get conf MOb.
 *
 * Looks for a message object with a given configuration. 
 *
 * \param[in] conf Configuration filter for message object.
 * \param[in] ignore Ignore number of found message objects.
 * 
 * \return Number of found message object. An invalid message object number
 * ( MOB_NUM ), if no message object is free.
 */
 MOb can_get_conf_mob(uint8_t conf, uint8_t ignore);
 
/*!
 * \brief Select MOb.
 *
 * Selects message object. They are shadowed, at any time only the 
 * selected one is in use, all MOb specific actions are only done on this. 
 *
 * \param[in] mob_id Identifier of message object.
 *
 * \return 0 on success.\n 
 * 1 otherwise.
 *
 * \note Uses auto increment index, starts at index = 0.
 */
inline uint8_t can_select_mob(MOb mob_id){
  if ( mob_id < CANARY_MOB_NUM ){
    CANPAGE = ((uint8_t) mob_id) << MOBNB0;
    return 0;
  }
  else{
    return 1;
  }
}

/*!
 * \brief Get free MOb.
 *
 * Looks for a free message object. A message object is free, if it is 
 * declared disabled in CANCDMOB ctl register.
 *
 * \param[in] ignore Ignore number of found message objects.
 *  
 * \return Number of found message object. An invalid message object number
 * ( MOB_NUM ), if no message object is free.
 */
inline MOb can_get_free_mob(uint8_t ignore){
	return can_get_conf_mob(CANARY_MOB_CONF_DISABLED, ignore);
}

/*!
 * \brief Get receiving MOb.
 *
 * Looks for a receiving message object. A message object is receiving, if it is 
 * declared receiving in CANCDMOB ctl register.
 *
 * \param[in] ignore Ignore number of found message objects.
 *  
 * \return Number of found message object. An invalid message object number
 * ( MOB_NUM ), if no message object is receiving.
 */
inline MOb can_get_recv_mob(uint8_t ignore){
	return can_get_conf_mob(CANARY_MOB_CONF_ENABLED_RECEPTION, ignore);
}

/*!
 * \brief Reset CAN port.
 *
 *  Resets CAN port by setting appropriate bits.
 */
inline void can_reset(){
  CANGCON = (1<<SWRES);
}

/*!
 * \brief Enable CAN port.
 *
 * Enable CAN port by setting appropriate bits.
 */
inline void can_enable(){
  CANGCON |= (1<<ENASTB);    
}

/*!
 * \brief Disable CAN port.
 *
 * Disable CAN port by setting appropriate bits.
 */
inline void can_disable(){
  CANGCON &= ~(1<<ENASTB);    
}

/*!
 * \brief Set bitrate.
 *
 * Sets bitrate. Necessary register values are taken from precompiler defines.
 *
 * \todo If informations are available about hardware latencies (e.g.
 * propagation times), this would be more convinient with one real baudrate 
 * value.
 */
inline void can_set_bitrate(){
  CANBT1 = CONF_CANBT1;
  CANBT2 = CONF_CANBT2;
  CANBT3 = CONF_CANBT3;
}

/*!
 * \brief Wait for rdy.
 *
 * Waits until CAN port is ready for further actions.
 */
inline void can_await_control_rdy(){
  while (!(CANGSTA & (1<<ENFG)));
}

/*
 * \brief Init all MObs.
 *
 * Initialze all message objects. Includes data, status, identifier and mask
 * registers.
 * 
 * \param[in] mode Which mode to use (only rx, all rx but one tx ...).
 *  
 * \return 0 In case of success.\n
 * 		   1 Otherwise.
 */
inline uint8_t can_init_mobs(enum CAN_CTRL_MODE mode){
	uint8_t i = 0;
	
  	// init all mobs
  	while(! can_select_mob(i++)){
		can_mob_init();
  	}
  	// TODO: enable other modes
  	switch(mode){
  		case CAN_CTRL_MODE_NONE:
  			return 0;
  			break;
  		case CAN_CTRL_MODE_ONE_TX:
  			i = 1;
  			break;
  		default:
			return 1;
  	}
	// enable rx mobs
	while ( i < CANARY_MOB_NUM ){
		if ( can_select_mob(can_get_free_mob(i++))){
			return 1;
		}
		else{
			can_mob_enable_rx();
		}
	}
	// TODO: no need (?) to enable tx mobs. used on demand FOR NOW! 
	return 0;
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

/*!
 * \brief Get highest prio MOb of irq.
 *
 * On interrupts enabled, it has to be determined, which message object
 * signalised an interrupt.
 *
 * \return Identifier of message object, which signalised something.
 * 0xF, if no mob has raised an interrupt.
 * 
 */
inline MOb can_get_irq_mob_highest(){
  return CANHPMOB >> HPMOB0;
}

/*!
 * \brief Get next MOb of irq.
 *
 * On interrupts enabled, it has to be determined, which message object
 * signalised an interrupt. 
 *
 * \param[in] ignore How many set bits ignore before.
 * \param[in] status Filter for message object. it's possible various message
 * objects are pending with different irq triggers.
 *
 * \return Identifier of message object, which signalised something.
 * 0xF, if no mob has raised an interrupt.
 * 
 */
inline MOb can_get_irq_mob(uint8_t ignore, uint8_t status){
	uint8_t i;
	uint8_t save;
	uint16_t cansit = (CANSIT1 << 8) | CANSIT2;
	
	save = CANPAGE;
	for ( i = 0; i < CANARY_MOB_NUM; i++, ignore++){
		if ( ignore == (CANARY_MOB_NUM) ){
			// wrap around
			ignore = 0;
		}
		if ( cansit & ( 1 << ignore) ){
			// found set bit in irq registers. check origin
			if ( ! can_select_mob(ignore)){
				if ( can_mob_get_status() & status){
					// status match. return on first match
					CANPAGE = save;
					return ignore;
				}
			}			
			
		}
	}
	CANPAGE = save;
	return CANARY_MOB_NUM;
}

/*!
 * \brief Enable irq for MOb.
 *
 * Enables interrupt generation for the given message object.
 *
 * \param[in] mob_id Identifier of message object.
 *
 * \note Interrupts have still be enabled globally and for the CAN 
 * controller.
 *
 * \sa enable_rx_irq
 */
inline void can_enable_mob_irq(MOb mob_id){
  CANIE1 |= ((1 << mob_id) >> 8 );
  CANIE2 |= (1 << mob_id);
}

/*!
 * \brief Disable irq for MOb.
 *
 * Disables interrupt generation of the given message object.
 *
 * \param[in] mob_id Identifier of message object.
 */
inline void can_disable_mob_irq(MOb mob_id){
  CANIE1 &= ~((1 << mob_id) >> 8 );
  CANIE2 &= ~(1 << mob_id);
}

#ifdef CONF_CAN_USE_RX_IRQ

/*!
 * \brief Enable receive irq.
 *
 * Enable generation of interrupts on message reception by setting 
 * appropriate bits.
 */
inline void can_enable_rx_irq(){
  CANGIE |= ((1<<ENIT) | (1<<ENRX));
}

/*!
 * \brief Disable receive irq.
 *
 * Disable generation of interrupts on message reception by setting 
 * appropriate bits.
 */
inline void can_disable_rx_irq(){
  CANGIE &= ~(1<<ENRX);
}

/*!
 * \brief CAN irq handler.
 * 
 * Handler of CAN irq's. 
 * 
 * \note  Used in conjunction with a redirecting table. Otherwise a direct
 * ISR macro could be used.
 */
void can_irq_handler();

/*!
 * \brief Set MOb rx handler.
 *
 * Set receiving handler. All disabled message objects are switched on for 
 * reeiving. 
 *
 * \param[in] handler Call-back receiving routine.
 * \param[in] mode Mode of buffer management.
 *
 * \return 0 on success. 
 * 1 otherwise.
 *
 */
uint8_t can_set_rx_handler(can_handler handler, enum CAN_IRQ_RX_MODE mode);

/*!
 * \brief Remove MOb rx handler.
 *
 * Remove receiving handler. All disabled message objects are switched off from 
 * reeiving. 
 *
 */
void can_remove_rx_handler();

#endif /* CONF_CAN_USE_RX_IRQ */

/*!
 * \brief Notify mob handlers.
 *
 * Notify all handlers of different events which were raised by a message
 * object.
 *
 * \param[in] mob_id Identifier of message object.
 *
 */
void can_notify_handler();

#endif /* CONF_CAN_USE_IRQ */

#endif /* __CAN_DRV_CAN128_H */
