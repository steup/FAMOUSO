function simviewcb(action)
global gh;
global scenario;
% SimWindow(UserData) - 1=running, 0=not running

%% action handling
switch action

    case 'init'
        set(gh.figNumber,'UserData',1);                 %Simulation runs
        %generate the calculation matrix based on the background picture
        cd saves;
        load scenario;
        matrix=imread(scenario.bmp_name,'bmp');           
        matrix_aux=logical(matrix);
        matrix_aux=~matrix_aux;
        dim=size(matrix_aux);
        matrix=zeros(dim(1:2));
        for i=1:3
           matrix=matrix | matrix_aux(:,:,i);
        end
        matrix=uint8(matrix);
        scenario.matrix=rot90(matrix,3);
        scenario.startTime=clock;
        save ('scenario','scenario');
        cd ..
        disp('Simulation started ...')
		simviewcb run;  

    case 'run'
        set(gh.menu.main,'Enable','on');        	
        set(gh.runmenu.main,'Enable','on');
        set(gh.runmenu.StopMenu,'Enable','on');
        set(gh.runmenu.StartMenu,'Enable','off');
        cd saves;
        load scenario;
        cd ..
        for i=1:length(scenario.robots)
            scenario.robots(i)=activate(scenario.robots(i));
        end
        set(gh.figNumber,'UserData',1);
        step=1;
        %%% -------------------------------------
        i=1;
%         t_main = timer('TimerFcn', ...
%            '[scenario.robots,scenario.matrix]=run(scenario.robots,scenario.matrix,1);',...
%            'ExecutionMode','fixedRate',...
%            'Period',0.05);
%        start(t_main);
       while (get(gh.figNumber,'UserData')==1)
           tic
           step = step + 1;
           output=sprintf('%2.3f',etime(clock,scenario.startTime));
           set(gh.StepText,'String',output);
           [scenario.robots,scenario.matrix]=run(scenario.robots,scenario.matrix,1);
           scenario=manipulate(scenario);
           aux=toc;
           if aux<scenario.period
                pause(scenario.period-aux);
           else
                disp('Defined period crossed !!!')
           end
       end
%        stop(t_main);

 	case 'stop'
        set(gh.figNumber,'UserData',0);
        disp('Simulation stoped ...')
        set(gh.runmenu.StartMenu,'Enable','on');
        
    case 'close'
        load gh.mat;
        delete(gh.figNumber);
        disp('Figure closed');
        disp('Aus Maus');

    otherwise
        error('Something wrong !!!')
end