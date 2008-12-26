function scenario = step(obj, event, i, handles)

global scenario
if ~scenario.robots(i).power && ~scenario.robots(i).crashed;
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
scenario.robots(i) = evaluate(scenario.robots(i), i ,scenario.matrix);
%   [scenario.robots(j),matrix,robots] = update(list(j),matrix,robots);	% Update robot
scenario.robots(i) = update(scenario.robots(i),i,scenario.matrix,scenario);

%% switch off timer objects of crashed robbies
if scenario.robots(i).crashed
    if scenario.robots(i).power
        aux=timerfind;
        name=sprintf('Robot_%i',i);
        in=strcmp(aux.Name,name);
        if sum(in)>0
            stop(aux(in));
            delete(aux(in));
            % local controlled robot
            fprintf('Robot %i crashed - corresponding timer switched off \n',i);
        else
            % via FAMOUSO controlled robot
            fprintf('Robot %i crashed \n',i);
        end
        scenario.robots(i).power=false;
        aux=get(handles.Active,'String');
        set(handles.Active,'String',num2str(str2double(aux)-1));
    end
end
