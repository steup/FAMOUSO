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

%dbstop in R1 at 25
disp('Loading Simulation ...')
simview;
simviewcb('load_configfile');
disp('Simulation started ...')
simviewcb('sim');