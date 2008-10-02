function new = alg_name(simrobot,matrix,step)

% your algorithm starts here

% sensor reading
   [dist,num] = readusonic(simrobot,'sensor_1',matrix);

% num, the nearest obstacle number, is not used    


   if dist<20
      simrobot = setvel(simrobot,[0 0.5]);  % turn left
   else
      simrobot = setvel(simrobot,[1 1]); % go straight on
   end

% end of your algorithm

new = simrobot;
