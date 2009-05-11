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

function positionCallback(channel_name)

global scenario

robotlist=[];

% eval(sprintf('global %s;',get_properties(channel_name,'comment')));
% aux=get_properties(channel_name,'comment');
% eval(sprintf('[values %s]=get_data(%s,''all'');',aux,aux));
% interpretation of the IDs and activation of the according step function

global Position
values=[];
[values Position]=get_data(Position,'all');
if isempty(values)
   return; 
end
if scenario.HumanDet==1
   return; 
end
for i=1:size(values,1)
    x=bitshift(double(values(i,2)),8)+double(values(i,3));
    y=bitshift(double(values(i,4)),8)+double(values(i,5));
    angle=bitshift(double(values(i,6)),8)+double(values(i,7));
    id=double(values(i,9));
end
robotID=find(scenario.robotIDs==id);
if isempty(robotID)
   return 
end

robotID = unique(robotID);
robot=scenario.robots(robotID);
matrix=scenario.matrix;

if robot.crashed==1
   return 
end

gain=0.4;

robot.position(1) = x*gain+robot.position(1)*(1-gain);
robot.position(2) = (480-y)*gain+robot.position(2)*(1-gain);
robot.heading = angle*gain+robot.heading*(1-gain);

i=robotID;
handles=[];

if ~robot.power && ~robot.crashed;
    robot=activate(robot,scenario.mode);
    disp(['Robot ' num2str(robot.number) ' activated ...']);
    robot.power=true;
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

% robot.position
% robot.heading
% dbstop in channel2robot.m at 29;

%% Move patch to new location
x=[robot.xdata robot.ydata];

aux=[[cos(robot.heading*pi/180) sin(robot.heading*pi/180)]; ...
    [-sin(robot.heading*pi/180) cos(robot.heading*pi/180)]];
conture=x*robot.scale*aux;

x_cp=robot.position(1)+conture(:,1);
y_cp=robot.position(2)+conture(:,2);

%     robot = set(robot,'patchXData',x_cp,'patchYData',y_cp);

if strcmp(scenario.mode,'sim')
    robot.patch.x=x_cp;
    robot.patch.y=y_cp;
else
    set(robot.patch,'XData',x_cp,'YData',y_cp);% Define the patch
end

%% Visualisation of each sensor detection area
sensors=robot.sensors;
scale=robot.scale;
resolution=20;
x_points=[];
y_points=[];
for i=1:max(size(sensors))
    arcCorner=sensors(i).position*scale*aux+robot.position;
    
    % decomposition of the arc
    angles = sensors(i).axisangle - sensors(i).scanangle/2:resolution:sensors(i).axisangle + sensors(i).scanangle/2;
    angles = angles*pi/180;
    
    arcElements=[];
    arcElements(1,:) = sensors(i).position(1) * scale + sensors(i).range * cos(angles);
    arcElements(2,:) = sensors(i).position(2) * scale + sensors(i).range * sin(angles);
    
    %         a=[];
    %         for j=1:length(arcElements)
    %  %           arcElements(:,j)=arcElements(:,j)'*aux+robot.position;
    %             a=[a; arcElements(:,j)'*aux+robot.position];
    %         end
    
    %         arcElements=arcElements'*aux+repmat(robot.position,length(arcElements),1);
    arcElements=arcElements'*aux+ones(length(arcElements),1)*robot.position;
    
    %         x_points=[x_points robot.position(1) arcCorner(1) arcElements(1,:) arcCorner(1)];
    %         y_points=[y_points robot.position(2) arcCorner(2) arcElements(2,:) arcCorner(2)];
    x_points=[x_points robot.position(1) arcCorner(1) arcElements(:,1)' arcCorner(1)];
    y_points=[y_points robot.position(2) arcCorner(2) arcElements(:,2)' arcCorner(2)];
    
end
%    set(robot,'lineXData',x_points,'lineYData',y_points);

if strcmp(scenario.mode,'sim')
    robot.line.x=x_points;
    robot.line.y=y_points;
else
    set(robot.line,'XData',x_points,'YData',y_points,'Color','r');
end


% ****** Place robot to matrix ****** !!!
% An dieser Stelle muessen crashes untersucht werden und der aktuelle
% Roboter in die Matrix eingef�gt
%
% Dabei werden zunaechst die Punkte in der Matrix bestimmt, auf die die
% Roboterkontur abzubilden ist, um danach zu pr�fen, ob es dort andere
% Eintragungen (1,2,3 ...) usw. gibt

% da hier xd nicht als "patch" betrachtet wird muss der Polygonzug noch
% geschlossen werden.

%%%%%%%% Crashdetection mit der Karte

if robot.potentialMapPatch(round(robot.position(1)),round(robot.position(2)))
    
    x_cp(length(x_cp)+1)=x_cp(1);
    y_cp(length(y_cp)+1)=y_cp(1);
    
    %plot(x_cp,y_cp,'-r');
    
    x_window=[floor(min(x_cp)) ceil(max(x_cp))];
    y_window=[floor(min(y_cp)) ceil(max(y_cp))];
    
    if x_window(1)<1
        x_window(1)=1;
    end
    if x_window(2)>size(matrix,2)
        x_window(2)=size(matrix,2);
    end
    if y_window(1)<1
        y_window(1)=1;
    end
    if y_window(2)>size(matrix,1)
        y_window(2)=size(matrix,1);
    end
    
    % ermittle alle nicht besetzten Felder des Window of Interest == crash
    % detection
    [x y] = find(matrix(x_window(1):x_window(2),y_window(1):y_window(2))==1);
    if ~isempty(x)
        x=x+x_window(1)-1;
        y=y+y_window(1)-1;
        in = myinpolygon(x,y,x_cp,y_cp);
        if ~isempty(in)
            robot.crashed=1;
            %             matrix(x(in),y(in))=robot.number;
        end
    end
end

%% Collisions of robots
if length(scenario.robots)>1
    for i=1:length(scenario.robots)
        if (i~=robotID)
            if sqrt((robot.position(1)-scenario.robots(i).position(1))^2+...
                    (robot.position(2)-scenario.robots(i).position(2))^2)<...
                    (scenario.robots(i).patchcircle+robot.patchcircle)
                %                 if abs(sum([robot.position -scenario.robots(i).position]))<...
                %                         1.4142*(scenario.robots(i).patchcircle+robot.patchcircle)
                if strcmp(class(scenario.robots(i).patch),'double')
                    patchXData = get(scenario.robots(i).patch,'XData');
                else
                    patchXData=scenario.robots(i).patch.x;
                end
                if isempty(patchXData)
                    break
                end
                if strcmp(class(scenario.robots(i).patch),'double')
                    patchYData = get(scenario.robots(i).patch,'YData');
                else
                    patchYData=scenario.robots(i).patch.y;
                end
                in=myinpolygon(patchXData,patchYData,x_cp,y_cp);
                if sum(in)~=0
                    scenario.robots(i).crashed=1;
                    robot.crashed=true;
                    break
                end
                in=myinpolygon(x_cp,y_cp,patchXData,patchYData);
                if sum(in)~=0
                    scenario.robots(i).crashed=true;
                    robot.crashed=true;
                    break
                end
            end
        end
    end
end
scenario.robots(robotID)=robot;