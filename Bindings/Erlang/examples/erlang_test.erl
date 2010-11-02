%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%
%% Copyright (c) 2008-2010 André Dietrich <dietrich@ivs.cs.uni-magdeburg.de>
%% All rights reserved.
%%
%%    Redistribution and use in source and binary forms, with or without
%%    modification, are permitted provided that the following conditions
%%    are met:
%%
%%    * Redistributions of source code must retain the above copyright
%%      notice, this list of conditions and the following disclaimer.
%%
%%    * Redistributions in binary form must reproduce the above copyright
%%      notice, this list of conditions and the following disclaimer in
%%      the documentation and/or other materials provided with the
%%      distribution.
%%
%%    * Neither the name of the copyright holders nor the names of
%%      contributors may be used to endorse or promote products derived
%%      from this software without specific prior written permission.
%%
%%
%%    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
%%    IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
%%    TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
%%    PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
%%    OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
%%    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
%%    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
%%    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
%%    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
%%    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
%%    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
%%
%%
%% $Id: template.m 1510 2010-02-02 07:18:58Z mschulze $
%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% Running this example:
%%
%% 1: start the erlang-shell
%%	$ erl
%%
%% 2: compile this example
%%	1> c(erlang_test).
%%
%% 3: run the example
%%	2> erlang_test:run().
%%
%% 4: quit the erlang-shell
%%	3> q().
%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


-module(erlang_test).

-include("../interface/famouso.hrl").

-export([run/0, init/0, init_publisher/0, publisher/1, init_subscriber/0, subscriber/1]).

-define(SUBJECT, "testsubj").
-define(DATA,    "1234567890").

% main function to run this example
run() ->
	init(),
	PublisherPid  = spawn(?MODULE, init_publisher,  []),
	SubscriberPid = spawn(?MODULE, init_subscriber, []),

	% wait for 15 seconds
	timer:sleep(15000),

	% stop the Publisher and Subscriber Process
	PublisherPid  ! stop,
	SubscriberPid ! stop.

% load the famouso interface
init() ->
	code:add_path("../interface/"),
	code:load_file(famouso).

init_publisher() ->
      EventChannel = famouso:event_channel(?SUBJECT),
      famouso:announce(EventChannel),
      publisher(EventChannel).

init_subscriber() ->
      EventChannel = famouso:event_channel(?SUBJECT),
      famouso:subscribe(EventChannel),
      subscriber(EventChannel).

% Publisher-Process transmits every 2 seconds
publisher(EventChannel) ->
      Timeout = 2000,
      receive  
		stop ->
			io:format("Publisher   --  Pid: ~p Subject: ~p    STOPPED~n", [self(), EventChannel#event_channel.subject]),
			famouso:unannounce(EventChannel)

		after Timeout ->
			io:format("Publisher   --  Pid: ~p Subject: ~p ~n", [self(), EventChannel#event_channel.subject]),
			famouso:publish(EventChannel, ?DATA),
			publisher(EventChannel)
      end.

% Subscriber-Process
subscriber(EventChannel) ->
	receive
		{event, Event} ->
			io:format("Subscriber  --  Pid: ~p Event: ~p~n", [self(), Event]),
			subscriber(EventChannel);
		stop -> 
			io:format("Subscriber  --  Pid: ~p Unsubscribtion~n", [self()]),
			famouso:unsubscribe(EventChannel);
		Msg ->
			io:format("Subscriber  --  Pid: ~p Msg: ~p~n", [self(), Msg]),
			subscriber(EventChannel)
	end.
