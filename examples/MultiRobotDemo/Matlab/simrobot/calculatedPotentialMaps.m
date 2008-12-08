function scenario=calculatedPotentialMaps(scenario)
%% Calculate the potential map for the patch
% according to the patch size the potentialMapPatch defines for each point
% the collision possibility 
for i=1:length(scenario.robots)
    num=floor(scenario.robots(i).patchcircle/sqrt(2));
    control_area=ones(2*num+1,2*num+1);
    scenario.robots(i).potentialMapPatch=filter2(control_area,scenario.matrix);
end

% hold on
% [x,y]=find(scenario.robots(1).potentialMapPatch);
% plot(x,y,'.r')
% disp(ready')

%     [x_obs y_obs] = find(scenario.matrix);
%     alpha=0:10:360;
%     for i=1:length(scenario.robots)
%         aux=zeros(size(scenario.matrix));
%         num=floor(scenario.robots(i).patchcircle/sqrt(2));
%         control_area=ones(2*num+1,2*num+1);
%             for j=1:length(x_obs)
%                 if x_obs(j)-num>0 &&  y_obs(j)-num>0 && ...
%                    x_obs(j)+num<size(scenario.matrix,1) && ...
%                    y_obs(j)+num<size(scenario.matrix,2)
%                    aux(x_obs(j)-num:x_obs(j)+num,y_obs(j)-num:y_obs(j)+num)= ... 
%                        aux(x_obs(j)-num:x_obs(j)+num,y_obs(j)-num:y_obs(j)+num) | control_area;
%                 end
%                 if mod(j,floor(length(x_obs)/20))==0
%                    fprintf('#'); 
%                 end
%             end
%             scenario.robots(i).potentialMapPatch=aux;
%             fprintf('\n');
%     end    
    
%     for i=1:length(x_free)
%         d = sqrt((x_free(i)*ones(length(x_obs),1)-x_obs).^2 + ...
%                  (y_free(i)*ones(length(y_obs),1)-y_obs).^2); % Compute distance for every point
%         scenario.distanceMap(x_free,y_free) = min(d); % Get the minimum distance
%         if mod(i,1000)
%             disp(num2str(i/length(x_free)));
%         end
%         i
%     end
%     toc
% %     for i=1:length(scenario.robots)
% %         scenario.robots(1).patchcircle
% %     end
% %    
%     [x,y]=find(scenario.matrix==1);
    
end