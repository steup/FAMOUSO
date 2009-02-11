function new = robot_T_app(simrobot, id, matrix)
global scenario
% your algorithm starts here

% sensor reading
   [dist,num] = readusonic(simrobot,id,'sensor_1',matrix);
   
   if scenario.FAMOUSO==1
       global Distance;
       aux=dist;
       if aux>=255
           aux=255;
       end
       publishing(Distance,[aux 118 id])
   end

% num, the nearest obstacle number, is not used    

   if dist<20
      simrobot = setvel(simrobot,[-0.5 0.5]);  % turn left
   else
      simrobot = setvel(simrobot,[0.7 0.7]); % go straight on
   end

% end of your algorithm

new = simrobot;
