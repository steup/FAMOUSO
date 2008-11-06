function scenario = step(scenario)

robots = ones(1,length(scenario.robots));

% Execute robots' algorithms
for j = 1:length(scenario.robots)
    %    profil on;
    scenario.robots(j) = feval(scenario.robots(j).af, ...
        scenario.robots(j),...
        scenario.matrix);	% Take robot from the list and execute algorithm
    %   [scenario.robots(j),matrix,robots] = update(list(j),matrix,robots);	% Update robot
    [scenario.robots(j),matrix,robots] = update(scenario.robots(j),matrix,robots);
    %    profil viewer;
    %    disp('Tralla')
end

	
% Disable crashed robots
if ~all(robots)
   [temp, off] = find(~robots);
   for i = 1:length(off)
      list(off(i)).crashed = 1;
   end
end