<?php

/*******************************************************************************
 *
 * Copyright (c) 2008-2010 André Dietrich <dietrich@ivs.cs.uni-magdeburg.de>
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
 * $Id: template.h 1510 2010-02-02 07:18:58Z mschulze $
 *
 ******************************************************************************/

require_once("FamousoDefines.php");
require_once("EventChannel.php");

/**
* Famouso-Subscriber
* @author André Dietrich
* @version 0.1
*/

class Subscriber{
	
	/** object of the eventchannel */	
	protected $m_EventChannel;

	/**
  	 * Constructer
  	 * @param EventChannel eventchannel for receiving new events 
  	*/	
	public function __construct( $EventChannel) {
		$this->m_EventChannel = $EventChannel;
	}

	/**
  	 * create socket and send subscribtion to the Famouso-EventChannelHandler
  	 * @return true, if subscribtion was succesful, else false 
  	*/
	public function subscribe() {
		if( $this->m_EventChannel->connect() ) {
			return $this->m_EventChannel->send(OP_SUBSCRIBE);
		}
		return false;
	}

	/**
  	 * get the next event by polling (socket is blocking)
  	 * @return the event = array('opcode' => , 'subject' =>, 'length' =>, 'data'=>);
  	*/
	public function getEvent() {
		return $this->m_EventChannel->receive();
	}

	/**
  	 * unsubscribing means losing the socket
  	*/
	public function unsubscribe() {
		$this->m_EventChannel->disconnect();
	}
}

?>