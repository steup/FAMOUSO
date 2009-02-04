function [ robot ] = evaluate( robot, i, matrix )
%EVALUATE Summary of this function goes here
%   Detailed explanation goes here
    robot = feval(robot.af, robot , i ,matrix);	
end
