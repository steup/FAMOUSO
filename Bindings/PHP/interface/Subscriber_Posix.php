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

require_once("Subscriber.php");
require_once("EventChannel.php");

/**
* Experimental Famouso-Subscriber with callback functionality.
* This kind of callback-handling works only with Posix compatible operating system.
* @author André Dietrich
* @version 0.1
*/

class Subscriber_Posix extends Subscriber{

	/** string or array of userdefined callback-function */
	protected $m_Callback;

	/** process-id of the child-process */
	protected $m_PidChild;
	/** process-id of the parent-process */
	protected $m_PidParent;

	/**
  	 * send subscribtion and fork the process, the child-process listens the socket for new events
  	 * while the parent-pocess continues working
  	 * @param $Callback stringrepresentation of the callback-function
  	 * @return true, if subscribtion and fork were succesful, else false 
  	*/
	public function subscribe( $Callback ) {
		
		if( ! parent::subscribe() ){
			return false;
		}

		$this->m_Callback  = $Callback;
		$this->m_PidParent = getmypid();
		$this->m_PidChild  = pcntl_fork();

		if ($this->m_PidChild == -1) {
			die('could not create child-process');
			return false;
		} else if ($this->m_PidChild) {
			// parent registers signal-handler
			pcntl_signal(SIGUSR1, array($this, 'sig_handler'));
		} else {
			// start listening at the socket
			$this->listen();		  
		}

		return true;
	}

	/**
  	 * signal handler is called from the child, if an event is received
  	 * and calls the userdefined callback 
  	 * @param $signo (waiting for signal SIGUSR1) 
  	*/
	public function sig_handler($signo) 
	{
		if($signo == SIGUSR1) {
			// call callback
			call_user_func($this->m_Callback, $this->getEvent() );
			// tell the child-process to go on
			posix_kill($this->m_PidChild, SIGCONT);
		}
	}

	/**
  	 * unsubscribing means losing the socket and kill the child-process
  	*/
	public function unsubscribe() {
		parent::unsubscribe();

		posix_kill($this->m_PidChild, SIGABRT);
	}
	
	/**
  	 * listening child-process
  	*/
	protected function listen(){
		while(true){
			/// if a famouso-message was received 
			if( $this->m_EventChannel->listen() ) {
				/// send signal to the parent
				posix_kill($this->m_PidParent, SIGUSR1);
				/// wait until the parent sends a signal to go on
				posix_kill(getmypid(), SIGSTOP);
			}
		}
	}
}

?>