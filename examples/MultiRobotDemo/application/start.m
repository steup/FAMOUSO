clear;
clear data;
%clear functions;
home;
setpaths;
disp('Lets go!');

FAMOUSOinit(TCPconfiguration());
FAMOUSOconnectAll();
FAMOUSOsubscribeAll();
FAMOUSOannounceAll();

disp('Loading Simulation ...')
simview;
simviewcb('load_configfile');
disp('Simulation started ...')
simviewcb('sim');