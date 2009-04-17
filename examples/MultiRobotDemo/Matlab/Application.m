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
function Application()

%% Initialisation of the System
setpaths;

%% Initialise FAMOUSO channels for input and output
FAMOUSOinit(TCPconfiguration());

%% Loading settings for TCP/IP communication
FAMOUSOconnectAll();
global distance
global velocity

%% Subscibtion/Announcement of all channels
% These functions only bundle the *subscribe(input_channel)* and
% *announce(output_channel)* calls
FAMOUSOsubscribeAll();
FAMOUSOannounceAll();

h=figure(1);
h=set(h,'Name','Graph');
hold on
xlim([0 100]);
ylim([0 30]);

robot=2;
publishing(velocity,[100 100 robot]);

%% Allication I - Writing Messages triggered by a timer in the background
t_publish = timer('TimerFcn', {@ObstacleAvoidance,distance,velocity,robot},...
           'ExecutionMode','fixedSpacing',...
           'Period',0.02);
start(t_publish);

function ObstacleAvoidance(obj, event, distance, velocity,robot)

persistent dist
if isempty(dist)
    dist=255;
end

[counter,distance]=get_event(distance);
data=double(get_data(distance,'all'));
%% Generating Commands
id=[];
if ~isempty(data)
    data;
    id=find(data(:,4)==robot);
end
if ~isempty(id)
    aux=sortrows(data(id,:),1);
    dist=aux(1,2); 
end 
v=[70 70 robot];
if dist<20
    v=[-20 20 robot];
end
publishing(velocity,v);
%% Visualisation
persistent i;
if isempty(i)
    i=zeros(100,1);
end
if ~isempty(id)
    i=i(2:100,:);
    if dist<255
        i(100)=dist;
    else
        i(100)=0;
    end
end
cla;
plot(i,'-r');
drawnow