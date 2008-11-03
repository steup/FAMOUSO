function [newlist,newmatrix] = run(list,matrix,step);
%[NEWMLIST,NEWMATRIX] = RUN(LIST,MATRIX)	one step of the simulation
%
%LIST is a list of all robots in the simulation
%
%MATRIX is a matrix of the 'environment', 
%       which takes the simulation place in
robots = ones(1,length(list));

% Execute robots' algorithms
for j = 1:length(list)

   list(j) = feval(list(j).af,list(j),matrix,step);	% Take robot from the list and execute algorithm
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