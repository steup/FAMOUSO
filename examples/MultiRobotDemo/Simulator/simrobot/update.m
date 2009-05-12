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

%% UPDATE	(system) updates the simulation.
function new = update(robotID,scenario)

robot=scenario.robots(robotID);
matrix=scenario.matrix;

if robot.power && ~robot.crashed
    robot.velocity(1) = robot.velocity(1) + robot.accel(1);
    robot.velocity(2) = robot.velocity(2) + robot.accel(2);
    
    R=robot.R;
    la=robot.la;
    lb=robot.lb;

    v=(R/(2*la))*[-lb*robot.velocity(1)+lb*robot.velocity(2);-la*robot.velocity(1)-la*robot.velocity(2)];

    heading = pi*robot.heading/180;
    cosa=cos(heading + pi);
    sina=sin(heading + pi);

    mat_aux=[[cosa sina];[cosa sina]];
    speed= v'*mat_aux;

    xs=speed(1);
    ys=speed(2);

    rotspd = ((R/(2*la))*(-robot.velocity(1) + robot.velocity(2))*180/pi);
           
    robot.position(1) = robot.position(1) + xs;	% Update x
    robot.position(2) = robot.position(2) + ys;	% Update y
    robot.heading = robot.heading + rotspd;		% Update heading
    if robot.heading >= 360
        robot.heading = robot.heading - 360;
    end
    if robot.heading < 0
        robot.heading = 360 + robot.heading;
    end
    
    %% Move patch to new location
    x=[robot.xdata robot.ydata];

    aux=[[cos(robot.heading*pi/180) sin(robot.heading*pi/180)]; ...
        [-sin(robot.heading*pi/180) cos(robot.heading*pi/180)]];
    conture=x*robot.scale*aux;
    
    x_cp=robot.position(1)+conture(:,1);
    y_cp=robot.position(2)+conture(:,2);
    
    try
        set(robot.patch,'XData',x_cp,'YData',y_cp);% Define the patch
    catch
        disp('Robot isnt active yet')
        return
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
        angles = sensors(i).axisangle - sensors(i).scanangle/2:sensors(i).resolution:sensors(i).axisangle + sensors(i).scanangle/2;
        angles = angles*pi/180;

        arcElements=[];
        arcElements(1,:) = sensors(i).position(1) * scale + sensors(i).range * cos(angles);
        arcElements(2,:) = sensors(i).position(2) * scale + sensors(i).range * sin(angles);
        
        arcElements=arcElements'*aux+ones(length(arcElements),1)*robot.position;
            
        x_points=[x_points robot.position(1) arcCorner(1) arcElements(:,1)' arcCorner(1)];
        y_points=[y_points robot.position(2) arcCorner(2) arcElements(:,2)' arcCorner(2)];
        
    end
    
        set(robot.line,'XData',x_points,'YData',y_points,'Color','r');
   
    
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
end

new = robot;