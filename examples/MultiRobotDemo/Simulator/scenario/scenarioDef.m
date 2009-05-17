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

function scenarioDef()
global scenario;

%% Definition of the scenario header
scenario.startTime=[];
scenario.matrix=[];
scenario.HumanDet=0;
scale=1;

%% Building the matrix based on a bitmap
scenario.bmp_name='schema.bmp';

% -------------------------------------------------------------------------
%% Robot construction site
% -------------------------------------------------------------------------

%% Definition of the sensor parameter
all_sensors=sensor_pool(scale);
sensors=all_sensors(5:7);
sensors(1).position=scale*[0 0];
sensors(2).position=scale*[0 0];
sensors(3).position=scale*[0 0];
sensors(4)=all_sensors(9);
sensors(4).position=scale*[0 0];

trigger=trigger_pool();

RobotIndexOffset=50;

%% ------> Robot 1
% Robot assambling 1
i=RobotIndexOffset;
scenario.robots=simrobot('robot_1',...
    i,...
    90,...
    'robot_P_app',...
    [1 1 0],...
    1,...
    scale*[-2 0 4 0 -2],...
    scale*[3 4 0 -4 -3],...
    sensors, ...
    [35 75], ...
    trigger(1),...
    [2 12 0]);

% %% ------> Robot 2
% Robot assambling 2
all_sensors=sensor_pool(scale);
sensors=all_sensors(8);
sensors(1).position=scale*[4 0];
i=length(scenario.robots)+1;
scenario.robots(i)=simrobot('robot_2',...
    52,...
    90,...
    'robot_C_app',...
    [1 0.5 0],...
    1,...
    scale*[-4 4 4 -4],...
    scale*[4 4 -4 -4],...
    sensors, ...
    [290 150], ...
    trigger(2), ...
    [2 12 0]);

% ------> Robot 3
% Robot assambling 3
all_sensors=sensor_pool(scale);
sensors=all_sensors(8);
sensors(1).position=scale*[4 0];
i=length(scenario.robots)+1;
scenario.robots(i)=simrobot('robot_3',...
    51,...
    90,...
    'robot_C_app',...
    [1 0.5 0],...
    1,...
    scale*[-4 4 4 -4],...
    scale*[4 4 -4 -4],...
    sensors, ...
    [290 150], ...
    trigger(2), ...
    [2 12 0]);

aux=[];
for i=1:length(scenario.robots)
    aux=[aux scenario.robots(i).number];
    if strcmp(scenario.robots(i).trigger.triggerMode,'timer')
        fprintf('Internal robot %d included\n',scenario.robots(i).number)
    else
        fprintf('External robot %d included\n',scenario.robots(i).number)
    end 
end
scenario.robotIDs=aux;