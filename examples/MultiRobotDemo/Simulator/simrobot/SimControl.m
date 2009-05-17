function varargout = SimControl(varargin)

% Last Modified by GUIDE v2.5 16-Apr-2009 17:45:26

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @SimControl_OpeningFcn, ...
                   'gui_OutputFcn',  @SimControl_OutputFcn, ...
                   'gui_LayoutFcn',  [], ...
                   'gui_Callback',   []);
if nargin && ischar(varargin{1})
   gui_State.gui_Callback = str2func(varargin{1});
end

if nargout
    [varargout{1:nargout}] = gui_mainfcn(gui_State, varargin{:});
else
    gui_mainfcn(gui_State, varargin{:});
end
% End initialization code - DO NOT EDIT


% --- Executes just before SimControl is made visible.
function SimControl_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   unrecognized PropertyName/PropertyValue pairs from the
%            command line (see VARARGIN)

% um beim Starten ohne "Stop" keinen Crash zu bauen
out = timerfind;
if ~isempty(out)
    stop(out);
    delete(out);
end
FAMOUSOdisconnectAll();

%% Load Uni logo
set(gcf,'CurrentAxes',handles.UniLogo)
logo=imread('OvGU_Logo_Fak_INF.jpg');
image(logo);
axis off

% Choose default command line output for SimControl
handles.output = hObject;

% Update handles structure
guidata(hObject, handles);

% UIWAIT makes SimControl wait for user response (see UIRESUME)
% uiwait(handles.figure1);
go_Callback(hObject, eventdata, handles)


% --- Outputs from this function are returned to the command line.
function varargout = SimControl_OutputFcn(hObject, eventdata, handles)
% varargout  cell array for returning output args (see VARARGOUT);
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Get default command line output from handles structure
varargout{1} = handles.output;


% --- Executes on button press in go.
function go_Callback(hObject, eventdata, handles)
% hObject    handle to go (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

home
global scenario;

%% Loading Scenario 
scenarioDef();
%% Preparing Map
preparingMaps();
calculatedPotentialMaps();
set(handles.file,'String',scenario.bmp_name);

%% Deleting all "old" robots
delete(findobj('type','line'));
delete(findobj('type','patch'));

%% Load Background Picture
set(gcf,'CurrentAxes',handles.SimulationWindow)
hold on;
axis off;
matrix=imread(scenario.bmp_name,'bmp');
for j=1:3
    matrix_aux(:,:,j) = flipud(matrix(:,:,j));
end
image(matrix_aux,'Parent',gca);

%% Starting FAMOUSO Middleware
if scenario.FAMOUSO==1
    aux=TCPconfiguration();
    FAMOUSOinit(aux);
    FAMOUSOconnectAll();
    FAMOUSOsubscribeAll();
    FAMOUSOannounceAll();
    set(handles.TCPIP,'String',aux.host);
end

handles=guihandles(findobj('name','control'));
set(handles.Active,'String',num2str(0));

%% Deleting all "old" robots
delete(findobj('type','line'));
delete(findobj('type','patch'));

scenario.startTime=clock;
disp('Simulation environment started ...');

%% time trigger for the time display
time_display = timer('TimerFcn',...
    {'timerdisplay',handles},...
    'Name','timerdisplay',...
    'ExecutionMode','fixedRate',...
    'Period',1);
start(time_display);

%% starting all timers (one for each robot)
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

% --- Executes on button press in stop.
function stop_Callback(hObject, eventdata, handles)
% hObject    handle to stop (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
global scenario;
out = timerfind;
if ~isempty(out)
    stop(out);
    delete(out);
end
if scenario.FAMOUSO==1
    FAMOUSOdisconnectAll();
    disp('Connections all closed')
end
for i=1:length(scenario.robots)
    scenario.robots(i).power=0;
end
disp('Simulation stoped');

% --- Executes when user attempts to close controlfigure.
function controlfigure_CloseRequestFcn(hObject, eventdata, handles)
% hObject    handle to controlfigure (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
stop_Callback(hObject, eventdata, handles)
% Hint: delete(hObject) closes the figure
delete(hObject);