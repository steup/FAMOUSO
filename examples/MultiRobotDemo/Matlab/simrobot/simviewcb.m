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
        matrix_bmp=imread(scenario.bmp_name,'bmp'); 
        matrix=ones(size(matrix_bmp,1),size(matrix_bmp,2));
        for i=1:size(matrix_bmp,1)
            for j=1:size(matrix_bmp,2)
                if ((matrix_bmp(i,j,1) == 255) && ...
                    (matrix_bmp(i,j,2) == 255) && ...
                    (matrix_bmp(i,j,3) == 255))
                    matrix(i,j)=0;
                end
            end
        end
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
       while (get(gh.figNumber,'UserData')==1)
           step = step + 1;
           output=sprintf('%2.3f',etime(clock,scenario.startTime));
           set(gh.StepText,'String',output);
           tic
           [scenario.robots,scenario.matrix]=run(scenario.robots,scenario.matrix,1);
           %scenario=manipulate(scenario);
           aux=toc;
           number(step-1)=toc;
           if aux<scenario.period
                pause(scenario.period-aux);
           else
                disp('Defined period crossed !!!')
           end
%            if step>50
%                 figure(2); 
%                 disp(['mean of calculation time ' num2str(mean(number))])
%                 hold on
%                 plot(number,'-g');
%                 plot([0 step],[scenario.period scenario.period],'-r')
%                 disp('stop')
%            end
       end
       disp('Main loop left ...')

 	case 'stop'
        set(gh.figNumber,'UserData',0);
        disp('Simulation stoped ...')
        set(gh.runmenu.StartMenu,'Enable','on');
        
    case 'close'
        delete(gh.figNumber);
        disp('Figure closed');
        disp('Aus Maus');

    otherwise
        error('Something wrong !!!')
end