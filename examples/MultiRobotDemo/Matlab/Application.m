%% Integration of a FAMOUSO interface in Matlab/Simulink
function Application()

%% Initialisation of the System
setpaths;

%% Initialise FAMOUSO channels for input and output
FAMOUSOinit(TCPconfiguration());

%% Loading settings for TCP/IP communication
FAMOUSOconnectAll();
global distance
global velocity

%% Subscibtion/Announcement of all channels
% These functions only bundle the *subscribe(input_channel)* and
% *announce(output_channel)* calls
FAMOUSOsubscribeAll();
FAMOUSOannounceAll();

h=figure(1);
h=set(h,'Name','Graph');
hold on
xlim([0 100]);
ylim([0 30]);

robot=2;
publishing(velocity,[100 100 robot]);

%% Allication I - Writing Messages triggered by a timer in the background
t_publish = timer('TimerFcn', {@ObstacleAvoidance,distance,velocity,robot},...
           'ExecutionMode','fixedSpacing',...
           'Period',0.02);
start(t_publish);

function ObstacleAvoidance(obj, event, distance, velocity,robot)

persistent dist
if isempty(dist)
    dist=255;
end

[counter,distance]=get_event(distance);
data=double(get_data(distance,'all'));
%% Generating Commands
id=[];
if ~isempty(data)
    data;
    id=find(data(:,4)==robot);
end
if ~isempty(id)
    aux=sortrows(data(id,:),1);
    dist=aux(1,2); 
end 
v=[70 70 robot];
if dist<20
    v=[-20 20 robot];
end
publishing(velocity,v);
%% Visualisation
persistent i;
if isempty(i)
    i=zeros(100,1);
end
if ~isempty(id)
    i=i(2:100,:);
    if dist<255
        i(100)=dist;
    else
        i(100)=0;
    end
end
cla;
plot(i,'-r');
drawnow