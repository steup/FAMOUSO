%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%
%% Copyright (c) 2008, 2009 Sebastian Zug <zug@ivs.cs.uni-magdeburg.de>
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

%% Integration of a FAMOUSO interface in Matlab/Simulink
% This programm illustrates the use of the FAMOUSO Matlab interface by
% several examples.
%
% Further questions to 
% <http://www-ivs.cs.uni-magdeburg.de/eos/de/ Otto-von-Guerike University Magdeburg>
%

%% Initialisation of the System
clear;
clear data;
home;
setpaths;

%% Initialise FAMOUSO channels for input and output
% The FAMOUSO channels are defined in the file
% *ECHconfiguration.m*. Each channel is represented by an instance of an
% input_channel/output_channel object, which name is determined by the
% "comment" entry. The structures *input_channel_IDs* *output_channel_IDs*
% contains the names and 64 Byte UIDs.
%
% The network connection parameter to the FAMOUSO ech
% are defined in *TCPconfiguration.m*
FAMOUSOinit(TCPconfiguration());

%% Object oriented representation
% After initialisastion you can use line / results / distance as Matlab objects, for instance
% for display, get, set, publish, announce/subscribe methods.
global distance
display(distance);

%% Loading settings for TCP/IP communication
FAMOUSOconnectAll();
global distance
global result;

%% Subscibtion/Announcement of all channels
% These functions only bundle the *subscribe(input_channel)* and
% *announce(output_channel)* calls
FAMOUSOsubscribeAll();
FAMOUSOannounceAll();

%% Application - Reading Messages via Callback
for i=1:20
    disp('doing something');
    pause(1);
end

%% Disconnection
% Unsubscibtion/Unannouncement is organised by FAMOUSO ECH
FAMOUSOdisconnectAll()

%% Close TCP/IP connection
disp('Aus Maus');