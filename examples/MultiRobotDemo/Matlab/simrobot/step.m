function scenario = step(scenario)

robots = ones(1,length(scenario.robots));

% Execute robots' algorithms
for j = 1:length(scenario.robots)
    % Take robot from the list and execute algorithm
    scenario.robots(j) = evaluate(scenario.robots(j), scenario.matrix);	
    %   [scenario.robots(j),matrix,robots] = update(list(j),matrix,robots);	% Update robot
    [scenario.robots(j),scenario.matrix,robots] = update(scenario.robots(j),scenario.matrix,robots);
end

	
% Disable crashed robots
if ~all(robots)
   [temp, off] = find(~robots);
   for i = 1:length(off)
      scenario.robots(off(i)).crashed = 1;
   end
end