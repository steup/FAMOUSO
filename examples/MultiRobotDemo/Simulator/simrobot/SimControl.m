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
home
global scenario;

% um beim Starten ohne "Stop" keinen Crash zu bauen
out = timerfind;
if ~isempty(out)
    stop(out);
    delete(out);
end
FAMOUSOdisconnectAll();

%% Loading Scenario and preparing Map
scenarioDef();
fprintf('Loading map ...')
matrix_bmp=imread(scenario.bmp_name,'bmp');
matrix=ones(size(matrix_bmp,1),size(matrix_bmp,2));
for i=1:size(matrix_bmp,1)
    for j=1:size(matrix_bmp,2)
        if ((matrix_bmp(i,j,1) == 255) && ...
            (matrix_bmp(i,j,2) == 255) && ...
            (matrix_bmp(i,j,3) == 255))
            matrix(i,j)=0;
        end
    end
end
matrix=rot90(matrix,3);
fprintf('                      [ok]\n');
% Filterung nach den Kantenübergängen
laplace=[[0 1 0];[1 -4 1];[0 1 0]];
contourline=conv2(laplace,matrix);
matrix=contourline(2:size(matrix,1)+1,2:size(matrix,2)+1);
[x_p y_p]=find(matrix==1 | matrix==2 );
scenario.matrix=zeros(size(matrix));
for i=1:length(x_p)
    scenario.matrix(x_p(i),y_p(i))=1;
end

set(handles.file,'String',scenario.bmp_name);

%% Deleting all "old" robots
delete(findobj('type','line'));
delete(findobj('type','patch'));

%% Calculation of seperate potential maps for each robot
scenario=calculatedPotentialMaps(scenario);

%% Construction of the legend
% offset_x=10;
% scale=1.8;
% dist_y=30*scale;
% dist_x=30;
% set(gcf,'CurrentAxes',handles.Legend)
% axis([-5 250 0 200])
% axis off;
% for i=1:length(scenario.robots)
%     handle=patch(scenario.robots(i).xdata*scale+offset_x,...
%                  scenario.robots(i).ydata*scale+i*dist_y,...
%                  scenario.robots(i).color);
%     set(handle,'HandleVisibility','off');
%     switch i
%         case 1
%             output='Simulierter Roboter';
%         case 2
%             output='Hardware in the Loop System';
%         case 3
%             output='Echter Roboter';
%         otherwise
%             output='?';
%     end
%     text(offset_x+dist_x,...
%         i*dist_y,...
%         output);
% end

%% Load Background Picture
set(gcf,'CurrentAxes',handles.SimulationWindow)
hold on;
axis off;
matrix=imread(scenario.bmp_name,'bmp');
for j=1:3
    matrix_aux(:,:,j) = flipud(matrix(:,:,j));
end
image(matrix_aux,'Parent',gca);

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

global scenario;

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

% %time trigger for the time display
time_display = timer('TimerFcn',...
    {'timerdisplay',handles},...
    'Name','timerdisplay',...
    'ExecutionMode','fixedRate',...
    'Period',1);
start(time_display);

%starting all timers (one for each robot)
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