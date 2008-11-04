clear variables;
home;
setpaths;
disp('Lets go!');

%% determine scenario source
source='file';
filename='scenario.mat';
% -----------------------------------------------------
global scenario;
switch source
    case 'file'
        cd saves
        eval(sprintf('load %s',filename));
    case 'new'
        cd scenario
        scenario=scenarioDef();
    otherwise
        disp('No idea what to do !!!')
end
cd ..

%% start 
mode='vis+sim';
switch mode
    % start simulation and visualisation mode
    case 'vis+sim'
        global gh;
        disp('Starting simulation environment ...');
        gh=simview(scenario);
        simviewcb('run');
    % simulation only mode    
    case 'sim'
        simviewcb('run');
    case 'vis'
        simviewcb('run');
end