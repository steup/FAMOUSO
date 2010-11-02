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

-module(famouso).
-include("famouso.hrl").
-author("André Dietrich <dietrich@ivs.cs.uni-magdeburg.de>").
-export([event_channel/1, init_event_handler/1, event_handler/3, subscribe/1, unsubscribe/1, announce/1, unannounce/1, publish/2]).

% local defines
-define(HOST, "localhost").
-define(PORT, 5005).

-define(OP_ANNOUNCE, "V").
-define(OP_PUBLISH,  "R").
-define(OP_SUBSCRIBE,"P").

event_channel(Subject) -> 
	Pid = spawn(?MODULE, init_event_handler, [Subject]),
	#event_channel{subject=Subject, pid=Pid}.

subscribe(EventChannel) ->
	EventChannel#event_channel.pid ! {subscribe, self()}.

unsubscribe(EventChannel) ->
	EventChannel#event_channel.pid ! stop.

announce(EventChannel) ->
	EventChannel#event_channel.pid ! {announce, self()}.

unannounce(EventChannel) ->
	EventChannel#event_channel.pid ! stop.

publish(EventChannel, Data) ->
	EventChannel#event_channel.pid ! {publish, Data}.

init_event_handler(Subject) ->
	{ok, Socket} = gen_tcp:connect(?HOST, ?PORT, [binary]),
	event_handler(Subject, Socket, []).

event_handler(Subject, Socket, Pid) ->
	% reset the socket for flow control
	inet:setopts(Socket, [{active, true}]),
	receive
		% do something with the data you receive
		{subscribe, Subscriber} ->
			Subscription = [?OP_SUBSCRIBE, Subject],
			ok = gen_tcp:send(Socket, Subscription),
			Subscriber ! ok,
			event_handler(Subject, Socket, Subscriber);

		{announce, Publisher} ->
			Announcement = [?OP_ANNOUNCE, Subject],
			Publisher ! ok,
			ok = gen_tcp:send(Socket, Announcement),
			event_handler(Subject, Socket, Publisher);

		{publish, Data} ->
			DataLength = length(Data),
			Msg = [	?OP_PUBLISH,
				Subject, 
				<<DataLength:32>>,
				Data],
			ok = gen_tcp:send(Socket, Msg),
			Pid ! ok,
			event_handler(Subject, Socket, Pid);

		stop ->
			ok = gen_tcp:close(Socket),
			Pid ! ok;

		% do something with the data you receive
		{tcp, Socket, Msg} ->
			<< _:8, _:64, Length:32, Data/binary>> = Msg,
			Event = #event{subject=Subject, length=Length, data=Data},
			Pid ! {event, Event},
			event_handler(Subject, Socket, Pid);
		{tcp_error, Socket, Reason} ->
			exit(Reason);
		% exit loop if the client disconnects
		{tcp_closed, Socket} ->
			io:format("~p Client Disconnected.~n", [erlang:localtime()]);

		Msg -> 
			io:format("Received ~p~n", [Msg]),
			event_handler(Subject, Socket, Pid)
	end.