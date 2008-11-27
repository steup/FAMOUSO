function control()

h1 = figure(...
'Units','characters',...
'PaperUnits',get(0,'defaultfigurePaperUnits'),...
'CloseRequestFcn',@controlfigure_CloseRequestFcn,...
'IntegerHandle','off',...
'InvertHardcopy',get(0,'defaultfigureInvertHardcopy'),...
'MenuBar','none',...
'Name','control',...
'NumberTitle','off',...
'PaperPosition',get(0,'defaultfigurePaperPosition'),...
'PaperSize',[20.98404194812 29.67743169791],...
'PaperType',get(0,'defaultfigurePaperType'),...
'Position',[103.8 39.6923076923077 91 21.7692307692308],...
'Resize','off',...
'Tag','controlfigure',...
'UserData',[],...
'Visible','on' );

% Adaptation of the window position
aux=get(h1,'Position');
ssize = get(0,'ScreenSize');
set(h1,'Position',[5 5 aux(3) aux(4)]);

h2 = uicontrol(...
'Parent',h1,...
'Units','characters',...
'FontSize',12,...
'Position',[3.20000000000001 18.9230769230769 41 1.92307692307692],...
'String','Control interface of Simrobot',...
'Style','text',...
'HandleVisibility','off',...
'BackgroundColor',get(h1,'Color'), ...
'Tag','title');

h3 = uicontrol(...
'Parent',h1,...
'Units','characters',...
'Callback',@go_Callback,...
'Position',[10 13.5384615384615 26 2.53846153846154],...
'String','GO',...
'BackgroundColor',get(h1,'Color'), ...
'Tag','go');

h4 = uicontrol(...
'Parent',h1,...
'Units','characters',...
'Callback',@stop_Callback,...
'Position',[10.2 10.1538461538462 25.6 2.69230769230769],...
'String','STOP',...
'BackgroundColor',get(h1,'Color'), ...
'Tag','stop');

h5 = uipanel(...
'Parent',h1,...
'Units','characters',...
'Title','Control',...
'HandleVisibility','off',...
'Tag','uipanel1',...
'Clipping','on',...
'BackgroundColor',get(h1,'Color'), ...
'Position',[3.00000000000001 0.692307692307695 40.8 17.0769230769231]);

h6 = uibuttongroup(...
'Parent',h5,...
'Units','characters',...
'Title','Configuration',...
'HandleVisibility','off',...
'Tag','Controlbox',...
'Clipping','on',...
'Position',[3 1.23076923076923 35.2 7.46153846153846],...
'SelectedObject',[],...
'BackgroundColor',get(h1,'Color'), ...
'SelectionChangeFcn',[],...
'OldSelectedObject',[]);

h7 = uicontrol(...
'Parent',h6,...
'Units','characters',...
'Position',[2.4 3.46153846153846 30.8 1.92307692307692],...
'String','Visualisation + Simulation',...
'Style','radiobutton',...
'Value',1,...
'BackgroundColor',get(h1,'Color'), ...
'Tag','Visualisation');

h8 = uicontrol(...
'Parent',h6,...
'Units','characters',...
'Position',[2.4 1.53846153846154 18.2 1.76923076923077],...
'String','Simulation only',...
'Style','radiobutton',...
'BackgroundColor',get(h1,'Color'), ...
'Tag','Simulation');

h9 = uipanel(...
'Parent',h1,...
'Units','characters',...
'Title','Active Systems',...
'HandleVisibility','off',...
'Tag','uipanel8',...
'Clipping','on',...
'BackgroundColor',get(h1,'Color'), ...
'Position',[46.8 13.9230769230769 40.8 3.76923076923077]);

h10 = uicontrol(...
'Parent',h9,...
'Units','characters',...
'FontWeight','bold',...
'Position',[3.8 0.538461538461537 32.4 1.53846153846154],...
'String','Active',...
'Style','text',...
'BackgroundColor',get(h1,'Color'), ...
'Tag','Active');

h11 = uipanel(...
'Parent',h1,...
'Units','characters',...
'Title','Current Time',...
'HandleVisibility','off',...
'Tag','uipanel9',...
'Clipping','on',...
'BackgroundColor',get(h1,'Color'), ...
'Position',[46.6 10 41.4 3.92307692307692]);

h12 = uicontrol(...
'Parent',h11,...
'Units','characters',...
'FontWeight','bold',...
'Position',[3.2 0.692307692307692 32.4 1.53846153846154],...
'String','Time',...
'Style','text',...
'BackgroundColor',get(h1,'Color'), ...
'Tag','Time');

h13 = uipanel(...
'Parent',h1,...
'Units','characters',...
'Title','TCP/IP',...
'HandleVisibility','off',...
'Tag','uipanel10',...
'Clipping','on',...
'BackgroundColor',get(h1,'Color'), ...
'Position',[46.6 6 41.4 3.92307692307692]);

h14 = uicontrol(...
'Parent',h13,...
'Units','characters',...
'FontWeight','bold',...
'Position',[3.2 0.692307692307692 32.4 1.53846153846154],...
'String','without FAMOUSO',...
'Style','text',...
'BackgroundColor',get(h1,'Color'), ...
'Tag','TCPIP');

go_Callback(h1);

% --- Executes on button press in go.
function go_Callback(hObject, eventdata, handles)
% hObject    handle to go (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

home
handles=guihandles(hObject);
set(handles.Time,'String',0);
global scenario;

switch scenario.source
    case 'file'
        cd saves
        aux=scenario.filename;
        disp(['Loading scenario specification from ' scenario.filename])
        eval(sprintf('load %s',scenario.filename));
        scenario.source='file';
        save(aux,'scenario');
    case 'new'
        scenarioDef();
    otherwise
        error('Unkown source')
end
cd ..

switch scenario.mode
    % start simulation and visualisation mode
    case 'vis+sim'
        disp('Starting simulation environment ...');
        gh=simview();
    % simulation only mode    
    case 'sim'
        disp('Simulation only')
    case 'vis'
        disp('have to be implemented')
    otherwise
        error('Unkown mode')
end

set(handles.Active,'String','Active');
if strcmp(scenario.mode,'sim')
    set(handles.Simulation,'Value',1);
else
    set(handles.Visualisation,'Value',1);
end

delete(findobj('type','line'));
delete(findobj('type','patch'));

if scenario.FAMOUSO==1
    FAMOUSOinit(TCPconfiguration());
    FAMOUSOconnectAll();
    FAMOUSOsubscribeAll();
    FAMOUSOannounceAll();
    aux=TCPconfiguration();
    set(handles.TCPIP,'String',aux.host);
end

scenario.startTime=clock;
disp('Simulator started ...')

% time trigger for the time display
time_display = timer('TimerFcn',...
    {'timerdisplay',handles},...
    'Name','timerdisplay',...
    'ExecutionMode','fixedRate',...
    'Period',0.05);
start(time_display);

% starting all timers (one for each robot)
for i=1:length(scenario.robots)
    if strcmp(scenario.robots(i).trigger.triggerMode,'timer')
        aux = timer('TimerFcn',...
            {'step' i handles},...
            'Name',sprintf('Robot_%i',i),...
            'StartDelay',scenario.robots(i).trigger.delay,...
            'ExecutionMode','fixedRate',...
            'Period',scenario.robots(i).trigger.period);
        start(aux);
    end
end

profile on
profile on

% --- Executes on button press in stop.
function stop_Callback(hObject, eventdata)
% hObject    handle to stop (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    global scenario;  
    if scenario.FAMOUSO==1
        FAMOUSOdisconnectAll();
        disp('Connections all closed')
    end
    out = timerfind;
    if ~isempty(out)
        stop(out);
        delete(out);
    end
    for i=1:length(scenario.robots)
        scenario.robots(i).power=0;
    end
    disp('Simulation stoped');

% --- Executes when user attempts to close controlfigure.
function controlfigure_CloseRequestFcn(hObject, eventdata)
    % hObject    handle to controlfigure (see GCBO)
    % eventdata  reserved - to be defined in a future version of MATLAB
    % handles    structure with handles and user data (see GUIDATA)
    stop_Callback(hObject, eventdata);
    handle=findobj('Tag','SimWindow');
    if ~isempty(handle)
        delete(handle);
    end
    delete(hObject);
    disp('Aus Maus')



