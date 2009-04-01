function scenario=calculatedPotentialMaps(scenario)
%% Calculate the potential map for the patch
% according to the patch size the potentialMapPatch defines for each point
% the collision possibility 
for i=1:length(scenario.robots)
    num=floor(scenario.robots(i).patchcircle/sqrt(2));
    num=floor(scenario.robots(i).patchcircle);
    control_area=ones(2*num+1,2*num+1);
    scenario.robots(i).potentialMapPatch=filter2(control_area,scenario.matrix);
end

% hold on
% [x,y]=find(scenario.robots(1).potentialMapPatch);
% plot(x,y,'.r')
% disp(ready')
    
end