function [newlist,newmatrix] = run(list,matrix,step);
%[NEWMLIST,NEWMATRIX] = RUN(LIST,MATRIX)	one step of the simulation
%
%LIST is a list of all robots in the simulation
%
%MATRIX is a matrix of the 'environment', 
%       which takes the simulation place in

robots = ones(1,length(list));

% Execute robots' algorithms

%% reading from FAMOUSO
global velocity;
global velocity_data;

[counter,velocity]=get_event(velocity);
if counter>0
    [velocity_data,velocity] = get_data(velocity, 'all');
else
    velocity_data=[]; 
end

%% Execution 

for j = 1:length(list)
   list(j) = feval(list(j).af,list(j),matrix,step);	% Take robot from the list and execute algorithm
%    aux= list(j).af;
%    aux=aux(1:max(size(aux))-2);
%    eval(sprintf('list(j)=%s(list(j),matrix,step);',aux));
   [list(j),matrix,robots] = update(list(j),matrix,robots);	% Update robot
end

	
% Disable crashed robots
if ~all(robots)
   [temp, off] = find(~robots);
   for i = 1:length(off)
      list(off(i)).crashed = 1;
   end
end


newlist = list;										% Return updated list
newmatrix = matrix;									% Return updated matrix