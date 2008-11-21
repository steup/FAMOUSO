function scenario = step(obj, event, i)
global scenario
% robots = ones(1,length(scenario.robots));

handles=guihandles(findobj('name','control'));

if get(scenario.robots(i),'power')==0 && get(scenario.robots(i),'crashed')==0;
    scenario.robots(i)=activate(scenario.robots(i),scenario.mode);
    disp(['Robot ' num2str(i) ' activated ...']);
    scenario.robots(i)=set(scenario.robots(i),'power',1);
    aux=get(handles.Active,'String');
    if ~strcmp(aux,'Active')
        set(handles.Active,'String',num2str(str2double(aux)+1));
    else
        set(handles.Active,'String',num2str(1));
    end
end


%% Execute robots' algorithms
% for j = 1:length(scenario.robots)
    % Take robot from the list and execute algorithm
    scenario.robots(i) = evaluate(scenario.robots(i), i ,scenario.matrix);	
    %   [scenario.robots(j),matrix,robots] = update(list(j),matrix,robots);	% Update robot
    [scenario.robots(i),scenario.matrix] = update(scenario.robots(i),scenario.matrix);
% end


%% switch off timer objects of crashed robbies
if get(scenario.robots(i),'crashed') && ...
   get(scenario.robots(i),'power')     
     aux=timerfind;
     name=sprintf('Robot_%i',i);
     in=strcmp(aux.Name,name);
     if sum(in)>0
         stop(aux(in));
         delete(aux(in));
         fprintf('Robot %i crashed - corresponding timer switched off \n',i);
     else
         fprintf('Robot %i crashed \n',i);
     end
     scenario.robots(i)=set(scenario.robots(i),'power',0);
     set(handles.Active,'String',num2str(str2double(aux)-1));
end