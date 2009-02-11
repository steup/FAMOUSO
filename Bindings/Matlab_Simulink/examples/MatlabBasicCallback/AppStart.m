%% Integration of a FAMOUSO interface in Matlab/Simulink
% This programm illustrates the use of the FAMOUSO Matlab interface by
% several examples.
%
% Further questions to 
% <http://www-ivs.cs.uni-magdeburg.de/eos/de/ Otto-von-Guerike University Magdeburg>
%

%% Initialisation of the System
clear;
clear data;
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

%% Loading settings for TCP/IP communication
FAMOUSOconnectAll();
global distance
global result;

%% Subscibtion/Announcement of all channels
% These functions only bundle the *subscribe(input_channel)* and
% *announce(output_channel)* calls
FAMOUSOsubscribeAll();
FAMOUSOannounceAll();

%% Application - Reading Messages via Callback
for i=1:20
    disp('doing something');
    pause(1);
end

%% Disconnection
% Unsubscibtion/Unannouncement is organised by FAMOUSO ECH
FAMOUSOdisconnectAll()

%% Close TCP/IP connection
disp('Aus Maus');