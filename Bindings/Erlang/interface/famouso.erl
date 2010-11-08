%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%
%% Copyright (c) 2008-2010 Andre Dietrich <dietrich@ivs.cs.uni-magdeburg.de>
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
%% $Id$
%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

-module(famouso).

%% @headerfile "famouso.hrl" 
-include("famouso.hrl").

-author("Andre Dietrich <dietrich@ivs.cs.uni-magdeburg.de>").
-export([event_channel/1, init_event_handler/1, event_handler/3, subscribe/1, unsubscribe/1, announce/1, unannounce/1, publish/2]).

%% local Famouso-definitions
-define(HOST, "localhost").
-define(PORT, 5005).

-define(OP_ANNOUNCE, "V").
-define(OP_PUBLISH,  "R").
-define(OP_SUBSCRIBE,"P").

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% @doc starts a new process as EventChannelHandler
%% @spec event_channel( string() ) -> event_channel()
event_channel(Subject) -> 
	Pid = spawn(?MODULE, init_event_handler, [Subject]),
	#event_channel{subject=Subject, pid=Pid}.

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% @doc send subscribtion-command to the EventChannelHandler 
%% @spec subscribe( event_channel() ) -> none()
subscribe(EventChannel) ->
	EventChannel#event_channel.pid ! {subscribe, self()}.

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% @doc send unsubscribtion-command to the EventChannelHandler
%% @spec unsubscribe( event_channel() ) -> none()
unsubscribe(EventChannel) ->
	EventChannel#event_channel.pid ! stop.

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% @doc send announce-command to the EventChannelHandler
%% @spec announce( event_channel() ) -> none()
announce(EventChannel) ->
	EventChannel#event_channel.pid ! {announce, self()}.

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% @doc send unannounce-command to the EventChannelHandler
%% @spec unannounce( event_channel() ) -> none()
unannounce(EventChannel) ->
	EventChannel#event_channel.pid ! stop.

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% @doc send data to the EventChannelHandler for publishing
%% @spec publish( event_channel() , binary() ) -> none()
publish(EventChannel, Data) ->
	EventChannel#event_channel.pid ! {publish, Data}.

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% @doc generates a socket and starts the event_handler routine
%% @spec init_event_handler( string(8) ) -> none()
%% @private
init_event_handler(Subject) ->
	{ok, Socket} = gen_tcp:connect(?HOST, ?PORT, [binary]),
	event_handler(Subject, Socket, []).

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% @doc eventhandler routine comunicates only with messages
%% @spec event_handler( string(8), Socket, pid() ) -> none()
%% @private
event_handler(Subject, Socket, Pid) ->
	% reset the socket for flow control
	inet:setopts(Socket, [{active, true}]),
	receive
		%% receive subscribtion and save subscriber pid
		{subscribe, Subscriber} ->
			Subscription = [?OP_SUBSCRIBE, Subject],
			ok = gen_tcp:send(Socket, Subscription),
			Subscriber ! ok,
			event_handler(Subject, Socket, Subscriber);
		%% receive announcement and save publisher pid
		{announce, Publisher} ->
			Announcement = [?OP_ANNOUNCE, Subject],
			Publisher ! ok,
			ok = gen_tcp:send(Socket, Announcement),
			event_handler(Subject, Socket, Publisher);
		%% receive message to publish data
		{publish, Data} ->
			%% generate famouso event
			DataLength = length(Data),
			Msg = [	?OP_PUBLISH, Subject, <<DataLength:32>>, Data],
			%% transmit the event
			ok = gen_tcp:send(Socket, Msg),
			Pid ! ok,
			event_handler(Subject, Socket, Pid);
		%% receive signal to stop the event_channel_handler
		stop ->
			ok = gen_tcp:close(Socket),
			Pid ! ok;
		%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		%% receive data from the socket 
		{tcp, Socket, Msg} ->
			<< _:8, _:64, Length:32, Data/binary>> = Msg,
			Event = #event{subject=Subject, length=Length, data=Data},
			Pid ! {event, Event},
			event_handler(Subject, Socket, Pid);
		%% tcp socket connection failes
		{tcp_error, Socket, Reason} ->
			exit(Reason);
		%% exit, socket connection was closed 
		{tcp_closed, Socket} ->
			io:format("~p Client Disconnected.~n", [erlang:localtime()]);
		%% default, for debuging purposes
		Msg -> 
			io:format("Received ~p~n", [Msg]),
			event_handler(Subject, Socket, Pid)
	end.
