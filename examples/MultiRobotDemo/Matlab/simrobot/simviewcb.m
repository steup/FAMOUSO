function simviewcb(action)
global gh;
% SimWindow(UserData) - 1=running, 0=not running

%% action handling
switch action
    case 'run'
        set(gh.menu.main,'Enable','on');
        set(gh.runmenu.main,'Enable','on');
        set(gh.runmenu.StopMenu,'Enable','on');
        set(gh.runmenu.StartMenu,'Enable','off');
        run_simrobot(gh);

 	case 'stop'
        scenario.running=0;
        disp('Simulation stoped ...')
        set(gh.runmenu.StartMenu,'Enable','on');
        
    case 'close'
        scenario.running=0;
        delete(gh.figNumber);
        disp('Figure closed');
        disp('Aus Maus');

    otherwise
        error('Something wrong !!!')
end