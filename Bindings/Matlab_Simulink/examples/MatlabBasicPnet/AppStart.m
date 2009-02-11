%% Integration of a FAMOUSO interface in Matlab/Simulink
% This programm illustrates the use of the FAMOUSO Matlab interface by
% several examples.
%
% Further questions to 
% <http://www-ivs.cs.uni-magdeburg.de/eos/de/ Otto-von-Guerike University Magdeburg>
%

%% Initialisation of the System
clear all;
clear functions;
home;
setpaths;

%% Initialise FAMOUSO channels for input and output
% The FAMOUSO channels are defined in the file
% *ECHconfiguration.m*. Each channel is represented by an instance of an
% input_channel/output_channel object, which name is determined by the
% "comment" entry. The structures *input_channel_IDs* *output_channel_IDs*
% contains the names and 64 Byte UIDs.
%
% The network connection parameter to the FAMOUSO ech
% are defined in *TCPconfiguration.m*
FAMOUSOinit(TCPconfiguration());

%% Object oriented representation
% After initialisastion you can use line / results / distance as Matlab objects, for instance
% for display, get, set, publish, announce/subscribe methods.
global distance
display(distance);

%% Establish the TCP/IP connection
FAMOUSOconnectAll();

%% Subscibtion/Announcement of all channels
% These functions only bundle the *subscribe(input_channel)* and
% *announce(output_channel)* calls
FAMOUSOsubscribeAll();
FAMOUSOannounceAll();

%% Allication I - Writing Messages triggered by a timer in the background
global result;
i=0;
t_publish = timer('TimerFcn', ...
            'publishing(result,[100+i 100 101 102 0 0 0 0]); i=i+1;disp(''event published'');',...
           'ExecutionMode','fixedSpacing',...
           'TasksToExecute',10,...
           'Period',0.5);
start(t_publish);
   
%% Application II - Reading Messages via Polling
% *get_events()* refreshes the data inside each input_channel object. For
% illustration the distance object was shown before and afterwards. The
% CurrentIndex points to the newest data record. The number of records is
% defined in *ECHconfiguration.m*. Currently the implementation offers
% tree different *get_data(input_channel)* methods:
% 
% * _newest_ - gets the last record
% * _mean_ - gives the mean value of the current records
% * _sum_ - outputs the sum of each payload byte of current records
% 
% The payload ist represented by characters and can be interpreted by the
% application (uint vs. int).

%display(distance);
for i=1:3
    counter=0;
    while ~counter
        [counter,distance]=get_event(distance);
        pause(0.01);
    end;
    disp(['Newest data: ' double(get_data(distance,'newest'))]);
end
display(distance);

%% Finish the time objects
if ~isempty(timerfind)
    stop(timerfind);
    delete(timerfind);
end

%% Disconnection
% Unsubscibtion/Unannouncement is organised by FAMOUSO ECH
FAMOUSOdisconnectAll()

%% Finish the program
disp('Aus Maus');