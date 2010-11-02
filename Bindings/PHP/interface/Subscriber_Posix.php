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

class Subscriber_Posix extends Subscriber{

	protected $m_Callback;

	protected $m_PidChild;
	protected $m_PidParent;

	public function subscribe( $Callback ) {
		
		if( ! parent::subscribe() ){
			return false;
		}

		$this->m_Callback  = $Callback;
		$this->m_PidParent = getmypid();
		$this->m_PidChild  = pcntl_fork();

		if ($this->m_PidChild == -1) {
			die('Konnte nicht verzweigen');
			return false;
		} else if ($this->m_PidChild) {
			pcntl_signal(SIGUSR1, array($this, 'sig_handler'));
		} else {
			$this->listen();		  
		}

		return true;
	}

	protected function callback($event) {
		call_user_func($this->m_Callback, $event);
	}

	public function sig_handler($signo) 
	{
		$this->callback( $this->getEvent() );
		posix_kill($this->m_PidChild, SIGCONT);
	}

	public function unsubscribe() {
		parent::unsubscribe();

		posix_kill($this->m_PidChild, SIGABRT);
	}

	protected function listen(){
		while(true){
			if( $this->m_EventChannel->listen() ) {
				posix_kill($this->m_PidParent, SIGUSR1);
				posix_kill(getmypid(), SIGSTOP);
			}
		}
	}
}

?>