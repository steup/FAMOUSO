%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%
%% Copyright (c) 2008-2010 Sebastian Zug <zug@ivs.cs.uni-magdeburg.de>
%% All rights reserved.
%%
%%    Redistribution and use in source and binary forms, with or without
%%    modification, are permitted provided that the following conditions
%%    are met:
%%
%%    * Redistributions of source code must retain the above copyright
%%      notice, this list of conditions and the following disclaimer.
%%
%%    * Redistributions in binary form must reproduce the above copyright
%%      notice, this list of conditions and the following disclaimer in
%%      the documentation and/or other materials provided with the
%%      distribution.
%%
%%    * Neither the name of the copyright holders nor the names of
%%      contributors may be used to endorse or promote products derived
%%      from this software without specific prior written permission.
%%
%%
%%    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
%%    IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
%%    TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
%%    PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
%%    OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
%%    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
%%    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
%%    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
%%    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
%%    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
%%    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
%%
%%
%% $Id$
%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
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
aux=findobj('Tag','SimFigure');
if ~isempty(aux)
    delete(aux);
end

handles.SimFigure =  figure('Name','SimFigure','Tag','SimFigure');
handles.SimAxes   =  gca;
set(gca,'Tag','SimAxes');
hold on;
axis off;
matrix=imread(scenario.bmp_name,'bmp');
for j=1:3
    matrix_aux(:,:,j) = flipud(matrix(:,:,j));
end
axis equal
handles.SimAxes=image(matrix_aux,'Parent',handles.SimAxes);
set(gca,'XLim',[1 size(matrix_aux,2)],'YLim',[1 size(matrix_aux,1)]);


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
