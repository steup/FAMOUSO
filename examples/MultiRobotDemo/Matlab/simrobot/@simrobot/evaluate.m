function [ robot ] = evaluate( robot, matrix )
%EVALUATE Summary of this function goes here
%   Detailed explanation goes here

    robot = feval(robot.af, robot ,matrix);	
end
