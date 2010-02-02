%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%
%% Copyright (c) 2008-2010 Sebastian Zug <zug@ivs.cs.uni-magdeburg.de>
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

function [dists,num] = readusonic(robot,robotID,sensname,matrix)
% READUSONIC 	reads data from an ultrasonic sensor.
%		See also READLASER.
global scenario

[tmp s] = size(robot.sensors);
i = 1;
while ((i < s) && (~strcmp(robot.sensors(i).name,sensname))),
   i = i + 1;
end
cosa=cos(robot.heading*pi/180);
sina=sin(robot.heading*pi/180);
aux=[[cosa sina]; ...
    [-sina cosa]];
pos=robot.sensors(i).position*robot.scale*aux+robot.position;

angles = robot.heading + robot.sensors(i).axisangle - robot.sensors(i).scanangle/2: ...
         robot.sensors(i).scanangle/2: ...
         robot.heading + robot.sensors(i).axisangle + robot.sensors(i).scanangle/2;
angles = angles*pi/180;

xs=[pos(1) pos(1)+cos(angles)*robot.sensors(i).range pos(1)];
ys=[pos(2) pos(2)+sin(angles)*robot.sensors(i).range pos(2)];

x_window=[floor(min(xs)) ceil(max(xs))];
y_window=[floor(min(ys)) ceil(max(ys))];

if x_window(1)<1
    x_window(1)=1;
end
if x_window(2)>size(matrix,1)
    x_window(2)=size(matrix,1);
end
if y_window(1)<1
    y_window(1)=1;
end
if y_window(2)>size(matrix,2)
    y_window(2)=size(matrix,2); 
end
%  plot([x_window(1) x_window(2) x_window(2) x_window(1) x_window(1)],...
%       [y_window(1) y_window(1) y_window(2) y_window(2) y_window(1)],'-g');

dists=999999;
num=1;

[x y] = find(matrix(x_window(1):x_window(2),y_window(1):y_window(2))~=0);

if ~isempty(x)
    x=x+x_window(1)-1;
    y=y+y_window(1)-1;
    
    in=myinpolygon(x,y,xs,ys);
    if (sum(in)>=1)
         dists =  min(sqrt((x(in)-pos(1)).^2+(y(in)-pos(2)).^2));
    end
end

if length(scenario.robots)>1
    for i=1:length(scenario.robots)
        if (i~=robotID)
%             if sqrt((robot.position(1)-scenario.robots(i).position(1))^2+...
%                     (robot.position(2)-scenario.robots(i).position(2))^2)<...
%                     (scenario.robots(i).sensorcircle+robot.sensorcircle)
            if abs(sum([robot.position -scenario.robots(i).position]))<...
               1.4142*(scenario.robots(i).sensorcircle+robot.sensorcircle)                    
%                if strcmp(class(scenario.robots(i).patch),'double')
                    patchXData = get(scenario.robots(i).patch,'XData');
 %               else
  %                  patchXData=scenario.robots(i).patch.x;
%                end
%                 if ~isempty(patchXData)
%                     break
%                 end
                if ~isempty(patchXData)
%                     if strcmp(class(scenario.robots(i).patch),'double')
                        patchYData = get(scenario.robots(i).patch,'YData');
%                     else
%                         patchYData=scenario.robots(i).patch.y;
%                     end
                    in=myinpolygon(patchXData,patchYData,xs,ys);
                    if sum(in)~=0
                        dists =  min(sqrt((patchXData(in)-pos(1)).^2+(patchYData(in)-pos(2)).^2));
                        num=i;
                    end
                end
            end
        end
    end
end