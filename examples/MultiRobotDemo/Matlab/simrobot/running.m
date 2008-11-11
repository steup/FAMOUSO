function running(obj, event, i, handles)
    global scenario
    if get(scenario.robots(i),'power')==0;
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
    scenario=step(scenario);
end