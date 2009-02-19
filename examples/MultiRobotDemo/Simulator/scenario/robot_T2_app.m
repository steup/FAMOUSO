function new = robot_T2_app(simrobot, id, matrix)
global scenario
% your algorithm starts here

% sensor reading
   [dist,num] = readusonic(simrobot,id,'sensor_1',matrix);
   [left,num] = readusonic(simrobot,id,'sensor_2',matrix);
   [right,num] = readusonic(simrobot,id,'sensor_3',matrix);
   

% num, the nearest obstacle number, is not used    

   if left<30
       simrobot = setvel(simrobot,[0.5 -0.5]);
   end
   if right<30
       simrobot = setvel(simrobot,[-0.5 0.5]);
   else  
   if dist<60
      simrobot = setvel(simrobot,[-0.5 0.5]);  % turn left
   else
      simrobot = setvel(simrobot,[0.7 0.7]); % go straight on
   end
   end

% end of your algorithm

new = simrobot;
