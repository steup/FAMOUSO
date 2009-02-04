function  step(obj, event, i, handles)
% tic
global scenario
robot=scenario.robots(i);

if ~robot.power && ~robot.crashed;
    scenario.robots(i)=activate(scenario.robots(i),scenario.mode);
    disp(['Robot ' num2str(i) ' activated ...']);
    scenario.robots(i).power=true;
    if isempty(handles)
        handles=guihandles(findobj('name','control'));
    end
    aux=get(handles.Active,'String');
    if ~strcmp(aux,'Active')
        set(handles.Active,'String',num2str(str2double(aux)+1));
    else
        set(handles.Active,'String',num2str(1));
    end
end

%% Execute robots' algorithms
% Take robot from the list and execute algorithm
if strcmp(scenario.robots(i).trigger.triggerMode,'timer')
    scenario.robots(i) = evaluate(scenario.robots(i), i ,scenario.matrix);
else
    [dist,num] = readusonic(scenario.robots(i),i,'sensor_1',scenario.matrix);
   if scenario.FAMOUSO==1
       global distance;
       aux=dist;
       if aux>=255
           aux=255;
       end
       publishing(distance,[aux 118 i]);
   end
end
% Update robot
scenario.robots(i) = update(i,scenario);

%% switch off timer objects of crashed robbies
if scenario.robots(i).crashed && scenario.robots(i).power
        aux=timerfind;
        name=sprintf('Robot_%i',i);
        in=strcmp(aux.Name,name);
        if sum(in)>0
            stop(aux(in));
            delete(aux(in));
            % local controlled robot
            fprintf('Robot %i crashed - corresponding timer switched off \n',i);
            aux=get(handles.Active,'String');
            set(handles.Active,'String',num2str(str2double(aux)-1));      
        else
            % via FAMOUSO controlled robot
            fprintf('Robot %i crashed \n',i);
            handles=findobj('Tag','Active');
            set(handles,'String',num2str(str2double(aux)-1));   
        end
         scenario.robots(i).power=false;  
end
% toc