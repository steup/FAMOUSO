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
* Famouso-Publisher
* @author André Dietrich
* @version 0.1
*/

class Publisher{

	/** object of the eventchannel */	
	protected $m_EventChannel;

	/**
  	 * Constructer
  	 * @param EventChannel eventchannel for publishing 
  	*/	
	public function __construct( $EventChannel ) {
		$this->m_EventChannel = $EventChannel;
	}

	/**
  	 * announce the subject
  	 * @return true if announcement was successful, else false 
  	*/
	public function announce() {

		/// open socket
		if( ! $this->m_EventChannel->connect()) {
			return false;
		}
		
		/// send announcement
		if( ! $this->m_EventChannel->send(OP_ANNOUNCE) ){
			return false;
		}

		return true;
	}

	/**
  	 * publish data via the eventchannel
  	 * @param data arbitrary data to publish
  	 * @return true if publishing was successful, else false 
  	*/
	public function publish($data) {
		return $this->m_EventChannel->send(OP_PUBLISH, $data);
	}

	/**
  	 * unannounce the subject 
  	*/
	public function unannounce() {
		/// close socket
		$this->m_EventChannel->disconnect();
	}
}

?>