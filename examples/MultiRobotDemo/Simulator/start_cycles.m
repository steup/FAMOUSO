function start_cycles()
%% Start the simrobot simulation environement 
% and a visualisation tool according to the configuration 
%
disp('Lets go!');
home;
setpaths;
%% Start respective environement
delete(timerfind)
global periode;
global iterations;
periode=5;          %[s] 
iterations=3;       %Zahl der Durchlaeufe

aux = timer('TimerFcn', @start_turn, ...
        'Name','testsystem',...
        'Period',periode*1.5,...
        'TasksToExecute',iterations,...
        'ExecutionMode','fixedSpacing');
start(aux);
end

function start_turn(obj, event)
    %% FAMOUSO integration?
    global scenario;
    scenario.FAMOUSO=0;    
    global periode;
    SimControl();
    disp('Simulation started')
    aux = timer('TimerFcn', @stop_turn, ...
        'Name','Stop_all',...
        'StartDelay',periode,...
        'ExecutionMode','singleShot');
    start(aux);
end

function stop_turn (obj, event)
    global scenario;
    global iterations;
    global periode;
    out = timerfind;
    if ~isempty(out)
        stop(out);
        delete(out);
    end
    for i=1:length(scenario.robots)
        scenario.robots(i).power=0;
    end
    disp('Simulation stoped');
    iterations=iterations-1;
    if iterations>0
        disp('---------------------------------------------------')
        fprintf('Cycle ... %i \n', iterations)
        disp('---------------------------------------------------')
        aux = timer('TimerFcn', @start_turn, ...
                'Name','testsystem',...
                'Period',periode*1.5,...
                'TasksToExecute',iterations,...
                'ExecutionMode','fixedSpacing');
        start(aux);

    else
        disp('Aus Maus')
    end
end