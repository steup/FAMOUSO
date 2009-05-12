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

function VelocityCallback(channel_name)
%tic
global scenario
robotlist=[];

%% Variable L�sung ...
% aux=get_properties(channel_name,'comment');
% eval(sprintf('global %s;',aux));
% eval(sprintf('[values %s]=get_data(%s,''all'');',aux,aux));

%% Spezifische Implementierung
global Velocity
[values Velocity]=get_data(Velocity,'all');

% interpretation of the IDs and activation of the according step function
for i=1:size(values,1)
    
     aux=char2int(values(i,:));    
     %% Achtung Achtung hier muss noch nachgebessert werden !!!
     robotid=aux(2);
     id=find(scenario.robotIDs==robotid);
     if isempty(id)
        break
     end
     robotlist=[robotlist id];
     k=1/20;
         left=double(typecast(int8(aux(3)), 'int8'))*k;
         right=double(typecast(int8(aux(4)), 'int8'))*k;
         scenario.robots(id)=setvel(scenario.robots(id),[left right]);

end

robotlist = unique(robotlist);
for i=1:length(robotlist)
    if ~scenario.robots(i).crashed
%         disp('just in front of step')
        step([],[],robotlist(i),[]);
    end
end
