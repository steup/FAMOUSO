function new = setpos(simrobot, position)
% SETPOS	(system) moves the robot to the new position.
%		See also GETPOS.

simrobot.position = position;

new = simrobot;