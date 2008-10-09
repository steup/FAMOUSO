clear;
clear data;
clear functions;
home;
setpaths;
disp('Lets go!');

FAMOUSOinit(TCPconfiguration());
FAMOUSOconnectAll();
FAMOUSOsubscribeAll();
FAMOUSOannounceAll();

%dbstop in get_event.m  at 24
disp('Loading Simulation ...')
simview;
simviewcb('load_configfile');
disp('Simulation started ...')
simviewcb('sim');