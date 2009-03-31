function new = robot_C_app(simrobot, id, matrix)

% your algorithm starts here

% sensor reading
    [dist,num] = readusonic(simrobot,id,'sensor_1',matrix);
   
    global Distance;
    aux=dist;
    if aux>=255
        aux=255;
    else
 %          disp('attention')
%          dbstop in channel2robot at 7
%         dist
    end
%      dist
    publishing(Distance,[simrobot.number 0 aux 0])

new = simrobot;
