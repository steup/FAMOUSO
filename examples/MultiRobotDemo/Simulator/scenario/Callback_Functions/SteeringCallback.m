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

function steeringCallback(channel_name)

global scenario

robotlist=[];

global Steering
values=[];
[values Position]=get_data(Steering,'all');
if isempty(values)
   return; 
end

if scenario.HumanDet==1
   return; 
end

for i=1:size(values,1)
    %% Achtung Achtung hier muss noch nachgebessert werden !!!
    robotid=double(typecast(int8(aux(1)), 'int8'));
    id=find(scenario.robotIDs==robotid);
    if robotid==255
        id=255; 
    end
    if isempty(id)
        break
    end
    robotlist=[robotlist id];
    k=1/10;
    left=double(typecast(int8(aux(3)), 'int8'))*k;
    right=double(typecast(int8(aux(4)), 'int8'))*k;
    if id~=255
        scenario.robots(id)=setvel(scenario.robots(id),[left right]);
        for i=1:length(scenario.robots)
            scenario.robots(i).panelControled=false;
        end
        scenario.robots(id).panelControled=true;
    else
        for i=1:length(scenario.robots)
        scenario.robots(i)=setvel(scenario.robots(i),[left right]);
        scenario.robots(i).panelControled=false;
        end
        scenario.robots(id).panelControled=true;
    end
end

robotlist = unique(robotlist);
for i=1:length(robotlist)
    if ~scenario.robots(i).crashed
        step([],[],robotlist(i),[]);
    end
end