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

/**
* EventChannel
* @author André Dietrich
* @version 0.1
*/


class EventChannel{

	/** subject string */
	protected $m_Subject;
	/** communication socket */
	protected $m_Socket;

	/**
  	 * Constructer
  	 * @param $Subject subject of the eventchannel 
  	*/	
	public function __construct( $Subject ) {
		$this->m_Subject = $Subject;
	}

	/**
	 * Creates a socket to communicate with the Famouso-EventChannelHandler 
  	 * @return true if connection established, else false 
  	*/	
	public function connect() {

		$this->m_Socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);

		if ( ! $this->m_Socket) {
			return false;
		}

		if( ! socket_connect($this->m_Socket, FAMOUSO_HOST, FAMOUSO_PORT) ) {
			return false;
		}

		return true;
	}

	/**
	 * close communication socket to the Famouso-EventChannelHandler 
  	 * @return true if connection established, else false 
  	*/
	public function disconnect() {
		socket_close($this->m_Socket);
	}

	/**
	 * send a meassage to the Famouso-EventChannelHandler
	 * @param $op one of the possible opcodes OP_ANNOUNCE|OP_PUBLISH|OP_SUBSCRIBE 
	 * (see FamousoDefines.php)
	 * @param $data data to send
  	 * @return true if sendig was succesful, else false 
  	*/
	public function send($op, $data="") {
		switch ($op) {
			case OP_ANNOUNCE:
			case OP_SUBSCRIBE: {
				$event = $op . $this->m_Subject;
				break;
			}			
			case OP_PUBLISH : {
				$event = OP_PUBLISH . $this->m_Subject . pack("N", strlen($data)) . $data;
				break;
			}
			default: {
				return false;
			}	
		}

		if( socket_write($this->m_Socket, $event, strlen($event)) ) {
			return true;
		}
		
		return false;
	}

	/**
	 * receives a message from the Famouso-EventChannelHandler and converts it to an associative array
  	 * @return $event = array('opcode' => , 'subject' =>, 'length' =>, 'data'=>);
  	*/
	public function receive() {
		$data = socket_read($this->m_Socket, MAX_PACKET_SIZE);
		
		$event = unpack("a1opcode/a8subject/Nlength/a*data", $data);

		return $event;
	}

	/**
	 * waits for a message from the Famouso-EventChannelHandler starting with opcode OP_PUBLISH
  	 * @return true if a message starting with OP_PUBLISH was received, else false
  	*/
	public function listen() {
		socket_recv($this->m_Socket, $buff, 1, MSG_PEEK);

		if($buff == OP_PUBLISH){
			return true;
		} else {
			return false;
		}
	}
}

?>