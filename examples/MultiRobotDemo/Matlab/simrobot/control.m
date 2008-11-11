function varargout = control(varargin)
%CONTROL M-file for control.fig
%      CONTROL, by itself, creates a new CONTROL or raises the existing
%      singleton*.
%
%      H = CONTROL returns the handle to a new CONTROL or the handle to
%      the existing singleton*.
%
%      CONTROL('Property','Value',...) creates a new CONTROL using the
%      given property value pairs. Unrecognized properties are passed via
%      varargin to control_OpeningFcn.  This calling syntax produces a
%      warning when there is an existing singleton*.
%
%      CONTROL('CALLBACK') and CONTROL('CALLBACK',hObject,...) call the
%      local function named CALLBACK in CONTROL.M with the given input
%      arguments.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Edit the above text to modify the response to help control

% Last Modified by GUIDE v2.5 05-Nov-2008 06:39:24

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
   'gui_Singleton',  gui_Singleton, ...
   'gui_OpeningFcn', @control_OpeningFcn, ...
   'gui_OutputFcn',  @control_OutputFcn, ...
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

% --- Executes just before control is made visible.
function control_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   unrecognized PropertyName/PropertyValue pairs from the
%            command line (see VARARGIN)

% Choose default command line output for control
handles.output = hObject;

% Update handles structure
guidata(hObject, handles);

handles=guidata(hObject);
aux=get(handles.controlfigure,'Position');
ssize = get(0,'ScreenSize');
set(handles.controlfigure, ...
'Position',[0 ssize(4)-aux(4) aux(3) aux(4)]);

% UIWAIT makes control wait for user response (see UIRESUME)
% uiwait(handles.controlfigure);

% --- Outputs from this function are returned to the command line.
function varargout = control_OutputFcn(hObject, eventdata, handles)
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

handles=guidata(hObject);
set(handles.Time,'String',0);
global scenario;

switch scenario.source
    case 'file'
        cd saves
        disp(['Loading scenario specification from ' scenario.filename])
        eval(sprintf('load %s',scenario.filename));
        scenario.source='file';
        save(scenario.filename,'scenario');
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

scenario.startTime=clock;
disp('Simulator started ...')

% time trigger for the time display
time_display = timer('TimerFcn',...
    {'timerdisplay',handles},...
    'ExecutionMode','fixedRate',...
    'Period',0.05);
start(time_display);

% starting all timers (one for each robot)
for i=1:length(scenario.robots)
    timerHandle(i) = timer('TimerFcn',...
        {'running' i,handles},...
        'StartDelay',get(scenario.robots(i),'trigger_delay'),...
        'ExecutionMode','fixedRate',...
        'Period',get(scenario.robots(i),'trigger_period'));
    start(timerHandle(i));
end

% --- Executes on button press in stop.
function stop_Callback(hObject, eventdata, handles)
% hObject    handle to stop (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
%setappdata(handles.stop,'Running',1);
    global scenario;
    out = timerfind;
    if ~isempty(out)
        stop(out);
        delete(out);
    end
    for i=1:length(scenario.robots)
        scenario.robots(i)=set(scenario.robots(i),'power',0);
    end
    disp('Simulation stoped');

% --- Executes when user attempts to close controlfigure.
function controlfigure_CloseRequestFcn(hObject, eventdata, handles)
    % hObject    handle to controlfigure (see GCBO)
    % eventdata  reserved - to be defined in a future version of MATLAB
    % handles    structure with handles and user data (see GUIDATA)

    stop_Callback(hObject, eventdata, handles);
    handle=findobj('Tag','SimWindow');
    if ~isempty(handle)
        delete(handle);
    end
    delete(hObject);
    disp('Aus Maus')