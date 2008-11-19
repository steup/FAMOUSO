function new = robot_T_app(simrobot, id, matrix)

% your algorithm starts here

% sensor reading
   [dist,num] = readusonic(simrobot,'sensor_1',matrix);
   
    global distance;
    aux=dist;
    if aux>=255
        aux=255;
    end
  
    publishing(distance,[aux 118 id])
% num, the nearest obstacle number, is not used    

   if dist<20
      simrobot = setvel(simrobot,[-0.5 0.5]);  % turn left
   else
      simrobot = setvel(simrobot,[0.7 0.7]); % go straight on
   end

% end of your algorithm

new = simrobot;
