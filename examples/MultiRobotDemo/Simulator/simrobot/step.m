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

function  step(obj, event, i, handles)
%tic
global scenario
if scenario.HumanDet==1
   return; 
end
robot=scenario.robots(i);

if ~robot.power && ~robot.crashed;
    scenario.robots(i)=activate(scenario.robots(i));
    disp(['Robot ' num2str(scenario.robots(i).number) ' activated ...']);
    scenario.robots(i).power=true;
    if isempty(handles)
        handles=guihandles(findobj('name','control'));
    end
    aux=get(handles.Active,'String');
    if ~strcmp(aux,'Active')
        set(handles.Active,'String',num2str(str2double(aux)+1));
    else
        set(handles.Active,'String',num2str(1));
    end
end

%% Execute robots' algorithms
if scenario.HumanDet~=1;

% Take robot from the list and execute algorithm
scenario.robots(i) = evaluate(scenario.robots(i), i ,scenario.matrix);
% Update robot
scenario.robots(i) = update(i,scenario);
end

%% switch off timer objects of crashed robbies
if scenario.robots(i).crashed && scenario.robots(i).power
    aux=timerfind;
    name=sprintf('Robot_%i',i);
    in=strcmp(aux.Name,name);
    if sum(in)>0
        stop(aux(in));
        delete(aux(in));
        % local controlled robot
        fprintf('Robot %i crashed - corresponding timer switched off \n',scenario.robots(i).number);
        aux=get(handles.Active,'String');
        set(handles.Active,'String',num2str(str2double(aux)-1));
    else
        % via FAMOUSO controlled robot
        fprintf('Robot %i crashed \n',scenario.robots(i).number);
        handles=findobj('Tag','Active');
        set(handles,'String',num2str(str2double(aux)-1));
    end
    scenario.robots(i).power=false;
    if scenario.FAMOUSO==1
        global Crashed_
        publishing(Crashed_,[scenario.robots(i).number]);
    end
end
%toc