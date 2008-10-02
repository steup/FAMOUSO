function new = jouda(simrobot,matrix,step)

% sensor reading
   [dist,num] = readusonic(simrobot,'sens1',matrix);


   if dist<20
      simrobot = setvel(simrobot,[0 0.5]);  % turn left
   else
      simrobot = setvel(simrobot,[0.5 0.5]); % go straight on
   end



new = simrobot;
