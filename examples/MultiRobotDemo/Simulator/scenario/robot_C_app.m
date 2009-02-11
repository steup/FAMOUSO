function new = robot_C_app(simrobot, id, matrix)

% your algorithm starts here

% sensor reading
    [dist,num] = readusonic(simrobot,id,'sensor_1',matrix);
   
    global distance;
    aux=dist;
    if aux>=255
        aux=255;
    else
%         disp('attention')
    end
%      dist
    publishing(distance,[aux 118 simrobot.number])

new = simrobot;
