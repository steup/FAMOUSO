function [new,newmatrix,newrobots] = update(simrobot,matrix,robots)
% UPDATE	(system) updates the simulation.


if (simrobot.power) & (~simrobot.crashed)
   
   simrobot.velocity(1) = simrobot.velocity(1) + simrobot.accel(1);
   simrobot.velocity(2) = simrobot.velocity(2) + simrobot.accel(2);
   
   [xs ,ys ,rotspd] = mmodel(simrobot.velocity(1), simrobot.velocity(2), simrobot.heading);
   
	xd = get(simrobot.patch,'XData');
	yd = get(simrobot.patch,'YData');

	% ***** Remove robot from matrix *****
	drmex(round([xd';yd']),matrix,robots,simrobot.number);

	myrotate(simrobot.patch,[0 0 1],rotspd,[simrobot.position(1) simrobot.position(2) 0])
   
   % Old position 
	xpos = simrobot.position(1);												
	ypos = simrobot.position(2);
	% ***** Move robot and store data  *****
	simrobot.position(1) = simrobot.position(1) + xs;	% Update x
	simrobot.position(2) = simrobot.position(2) + ys;	% Update y
	simrobot.heading = simrobot.heading + rotspd;		% Update heading
   if simrobot.heading >= 360
      simrobot.heading = simrobot.heading - 360;
   end
   if simrobot.heading < 0
      simrobot.heading = 360 + simrobot.heading;
   end

	xd = get(simrobot.patch,'XData');
	yd = get(simrobot.patch,'YData');

	% Move patch to new location
	xd = xd - xpos;																
	yd = yd - ypos;	
	xd = simrobot.position(1) + xd;
	yd = simrobot.position(2) + yd;

	set(simrobot.patch,'XData',xd,'YData',yd)
    
    %% Visualisation of each sensor detection area
    sensors=simrobot.sensors;
    scale=simrobot.scale;
    x_points=[];
    y_points=[];
    for i=1:max(size(sensors))
      aux=[[cos(simrobot.heading*pi/180) sin(simrobot.heading*pi/180)]; ...
           [-sin(simrobot.heading*pi/180) cos(simrobot.heading*pi/180)]];
      arcCorner=sensors(i).position*scale*aux+simrobot.position;
      
      % decomposition of the arc
      angles = sensors(i).axisangle - sensors(i).scanangle/2:5:sensors(i).axisangle + sensors(i).scanangle/2;
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
    set(simrobot.line,'XData',x_points,'YData',y_points,'Color','r');
    
    
    % ****** Place robot to matrix ******
   prmex(round([xd';yd']),matrix,robots,simrobot.number);

   
end

%% ?
% We need to store robot's position
%% I switch this off !!! It needs a lot of performance 
%simrobot.history = [simrobot.history ; simrobot.position simrobot.heading];

newrobots = robots;
new = simrobot;
newmatrix = matrix;
