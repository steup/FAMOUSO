function [new,newmatrix,newrobots] = update(simrobot,matrix,robots)
% UPDATE	(system) updates the simulation.

if (simrobot.power) && (~simrobot.crashed)
    % ***** Remove robot from matrix *****
%     profile on

    xd = get(simrobot,'patchXData');
    yd = get(simrobot,'patchYData');
    
    x_window=[floor(min(xd)) ceil(max(xd))];
    y_window=[floor(min(yd)) ceil(max(yd))];

    [x,y]=find(matrix(x_window(1):x_window(2),y_window(1):y_window(2))==simrobot.number);
    x=x+x_window(1)-1;
    y=y+y_window(1)-1;
    if ~isempty(x)
        matrix(x,y)=0;
    end

    %% ***** Move robot and store data  ***** 
    simrobot.velocity(1) = simrobot.velocity(1) + simrobot.accel(1);
    simrobot.velocity(2) = simrobot.velocity(2) + simrobot.accel(2);
    
    % Model constants, keep consistent with those ones in invmodel.m!
    R = 1;
    la = 3;
    lb = 0;

    v=(R/(2*la))*[[-lb*simrobot.velocity(1)+lb*simrobot.velocity(2)];[-la*simrobot.velocity(1)-la*simrobot.velocity(2)]];

    heading = pi*simrobot.heading/180;
    cosa=cos(heading + pi);
    sina=sin(heading + pi);

    mat_aux=[[cosa sina];[cosa sina]];
    speed= v'*mat_aux;

    xs=speed(1);
    ys=speed(2);

    rotspd = ((R/(2*la))*(-simrobot.velocity(1) + simrobot.velocity(2))*180/pi);
           
    simrobot.position(1) = simrobot.position(1) + xs;	% Update x
    simrobot.position(2) = simrobot.position(2) + ys;	% Update y
    simrobot.heading = simrobot.heading + rotspd;		% Update heading
    if simrobot.heading >= 360
        simrobot.heading = simrobot.heading - 360;
    end
    if simrobot.heading < 0
        simrobot.heading = 360 + simrobot.heading;
    end
    
    %% Move patch to new location
    x=[get(simrobot,'xdata') get(simrobot,'ydata')];

    aux=[[cos(simrobot.heading*pi/180) sin(simrobot.heading*pi/180)]; ...
        [-sin(simrobot.heading*pi/180) cos(simrobot.heading*pi/180)]];
    conture=x*simrobot.scale*aux;

    set(simrobot,'patchXData',simrobot.position(1)+conture(:,1),...
                 'patchYData',simrobot.position(2)+conture(:,2));

    %% Visualisation of each sensor detection area
    sensors=simrobot.sensors;
    scale=simrobot.scale;
    resolution=20;
    x_points=[];
    y_points=[];
    for i=1:max(size(sensors))
        arcCorner=sensors(i).position*scale*aux+simrobot.position;

        % decomposition of the arc
        angles = sensors(i).axisangle - sensors(i).scanangle/2:resolution:sensors(i).axisangle + sensors(i).scanangle/2;
        angles = angles*pi/180;

        arcElements=[];
        arcElements(1,:) = sensors(i).position(1) * scale + sensors(i).range * cos(angles);
        arcElements(2,:) = sensors(i).position(2) * scale + sensors(i).range * sin(angles);

        for j=1:length(arcElements)
            arcElements(:,j)=arcElements(:,j)'*aux+simrobot.position;
        end
        x_points=[x_points simrobot.position(1) arcCorner(1) arcElements(1,:) arcCorner(1)];
        y_points=[y_points simrobot.position(2) arcCorner(2) arcElements(2,:) arcCorner(2)];
    end
    set(simrobot,'lineXData',x_points,'lineYData',y_points);
    
    % ****** Place robot to matrix ****** !!!
    %prmex(round([xd';yd']),matrix,robots,simrobot.number);
    % An dieser Stelle muessen crashes untersucht werden und der aktuelle
    % Roboter in die Matrix eingefügt
    %
    % Dabei werden zunaechst die Punkte in der Matrix bestimmt, auf die die
    % Roboterkontur abzubilden ist, um danach zu prüfen, ob es dort andere
    % Eintragungen (1,2,3 ...) usw. gibt
    
    % da hier xd nicht als "patch" betrachtet wird muss der Polygonzug noch
    % geschlossen werden.
    
    xd(length(xd)+1)=xd(1);
    yd(length(yd)+1)=yd(1);
    
    %plot(xd,yd,'-r');
    
    x_window=[floor(min(xd)) ceil(max(xd))];
    y_window=[floor(min(yd)) ceil(max(yd))];

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
    
    % ermittle alle nicht leeren Felder das Window of Interest == crash
    % detection
    [x y] = find(matrix(x_window(1):x_window(2),y_window(1):y_window(2))~=0);
    if ~isempty(x)
        x=x+x_window(1)-1;
        y=y+y_window(1)-1;
        in = myinpolygon(x,y,xd,yd);
        if ~isempty(in)
            simrobot.crashed=1;
            matrix(x(in),y(in))=simrobot.number;
        end
    end
    [x y] = find(matrix(x_window(1):x_window(2),y_window(1):y_window(2))==0);
    if ~isempty(x)
        x=x+x_window(1)-1;
        y=y+y_window(1)-1;
        in = myinpolygon(x,y,xd,yd);
        matrix(x(in),y(in))=simrobot.number;
    end
   
end

newrobots = robots;
new = simrobot;
newmatrix = matrix;