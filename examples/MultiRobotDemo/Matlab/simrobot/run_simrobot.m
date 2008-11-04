function run_simrobot(varargin)
%RUN Summary of this function goes here
%   Detailed explanation goes here

    global scenario;

    if ~isempty(varargin)
        gh=varargin{1};
    else
       gh=[]; 
    end
    cd saves;
    load scenario;
    cd ..
    for i=1:length(scenario.robots)
        scenario.robots(i)=activate(scenario.robots(i));
    end
    step=1;
    scenario.startTime=clock;
    scenario.running=1;
    while (scenario.running==1)
        step = step + 1;
        if ~isempty(gh)
            output=sprintf('%2.3f',etime(clock,scenario.startTime));
            set(gh.StepText,'String',output);
        end
        tic
        [scenario.robots,scenario.matrix]=run(scenario.robots,scenario.matrix,1);
        %scenario=manipulate(scenario);
        aux=toc;
        number(step-1)=toc;
        if aux<scenario.period
            pause(scenario.period-aux);
            disp('here')
        else
            disp('Defined period crossed !!!')
        end
    end

end
