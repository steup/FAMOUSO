function start()
%% Start the simrobot simulation environement 
%and a visualisation tool according to the configuration 
%
% source = 'new'    
%       Building a new scenario stucture with a new matrix, a new number of
%       robots etc.
% source = 'file'
%       Reading structure from a .mat file
% mode = 'sim'
% mode = 'vis'
% mode = 'vis+sim'

clear variables;
home;
setpaths;
disp('Lets go!');

%% Opening control GUI
aux=control;
ch=guidata(aux);

%% determine scenario source
source='file';
source='new';
filename='scenarioFile.mat';
% --------------------------------------------------
switch source
    case 'file'
        cd saves
        eval(sprintf('load %s',filename));
    case 'new'
        scenario=scenarioDef(filename);
    otherwise
        error('Unkown source')
end
cd ..

%% Start respective environement 
gh=[];
scenario.mode='sim';
scenario.mode='vis+sim';
setappdata(aux,'scenario',scenario);
switch scenario.mode
    % start simulation and visualisation mode
    case 'vis+sim'
        disp('Starting simulation environment ...');
        gh=simview(scenario);
        setappdata(aux,'gh',gh);
        control('go_Callback',aux);
    % simulation only mode    
    case 'sim'
        disp('Simulation only')
        control('go_Callback',aux);
    case 'vis'
        disp('have to be implemented')
    otherwise
        error('Unkown mode')
end