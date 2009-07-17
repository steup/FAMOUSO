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

function sensors = sensor_pool(scale)

sensors.name='sensor_1';
%sensors.position=scale*[4 0];
sensors.position=[0 0];
sensors.axisangle=5;
sensors.scanangle=50;
sensors.range=scale*30;
sensors.resolution=1;
sensors.line=0;

i=length(sensors)+1;
sensors(i).name='sensor_2';
% sensors(i).position=scale*[2 2];
sensors(i).position=[0 0];
sensors(i).axisangle=55;
sensors(i).scanangle=100;
sensors(i).range=scale*20;
sensors(i).resolution=10;
sensors(i).line=0;

i=length(sensors)+1;
sensors(i).name='sensor_3';
% sensors(i).position=scale*[2 -2];
sensors(i).position=[0 0];
sensors(i).axisangle=-50;
sensors(i).scanangle=110;
sensors(i).range=scale*20;
sensors(i).resolution=10;
sensors(i).line=0;

%% Sensor f�r die Simulierten Roboter
i=length(sensors)+1;
sensors(i).name='sensor_4';
% sensors(i).position=scale*[3 -7];
sensors(i).position=[0 0];
sensors(i).axisangle=-50;
sensors(i).scanangle=110;
sensors(i).range=scale*20;
sensors(i).resolution=10;
sensors(i).line=0;

%% Sensoren f�r die Positionssch�tzung ...
i=length(sensors)+1;
sensors(i).name='sensor_1';
sensors(i).position=scale*[4 0];
sensors(i).axisangle=0;
sensors(i).scanangle=5;
sensors(i).range=scale*200;
sensors(i).resolution=1;
sensors(i).line=0;

i=length(sensors)+1;
sensors(i).name='sensor_2';
sensors(i).position=scale*[2 2];
sensors(i).axisangle=90;
sensors(i).scanangle=5;
sensors(i).range=scale*200;
sensors(i).resolution=1;
sensors(i).line=0;

i=length(sensors)+1;
sensors(i).name='sensor_3';
sensors(i).position=scale*[2 -2];
sensors(i).axisangle=-90;
sensors(i).scanangle=5;
sensors(i).range=scale*200;
sensors(i).resolution=1;
sensors(i).line=0;

%% Sensor fuer reale Roboter
i=length(sensors)+1;
sensors(i).name='sensor_1';
sensors(i).position=scale*[5 0];
sensors(i).axisangle=0;
sensors(i).scanangle=110;
sensors(i).range=scale*20;
sensors(i).resolution=10;
sensors(i).line=0;

%% Bumper Rundum
i=length(sensors)+1;
sensors(i).name='sensor_40';
sensors(i).position=scale*[0 0];
sensors(i).axisangle=0;
sensors(i).scanangle=120;
sensors(i).range=scale*15;
sensors(i).resolution=10;
sensors(i).line=0;

