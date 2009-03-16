function new = robot_T2_app(simrobot, id, matrix)
global scenario
% your algorithm starts here

% sensor reading
[dist,num] = readusonic(simrobot,id,'sensor_1',matrix);
[left,num] = readusonic(simrobot,id,'sensor_2',matrix);
[right,num] = readusonic(simrobot,id,'sensor_3',matrix);

% num, the nearest obstacle number, is not used

if simrobot.panelControled==false
    if left<50
        simrobot = setvel(simrobot,[0.5 -0.5]);
    end
    if right<50
        simrobot = setvel(simrobot,[-0.5 0.5]);
    else
        if dist<80
            simrobot = setvel(simrobot,[-0.5 0.5]);  % turn left
        else
            simrobot = setvel(simrobot,[1 1]); % go straight on
        end
    end
end
% end of your algorithm

new = simrobot;
