function gh=simview();
global scenario;

%% close all related windows
h = findobj('Tag','SimWindow');
delete(h);

%% Create a new simulator figure
gh.figNumber = figure;

ssize = get(0,'ScreenSize');
set(gh.figNumber, ...
    'Position',[ssize(3)/2 ssize(4)*0.1 ssize(3)/2-5 ssize(4)*0.8],...
    'NumberTitle','off',...
    'CloseRequestFcn',@my_closereq,...
    'Name','Simulation Window',...
    'RendererMode','manual',...
    'Renderer','Painters',...
    'Backingstore','off',...
    'Tag','SimWindow');

%% add legend
offset_x=100;
offset_y=-200;
dist_y=30;
dist_x=30;
for i=1:length(scenario.robots)
    patch(scenario.robots(i).xdata+offset_x,scenario.robots(i).ydata+offset_y+i*dist_y,scenario.robots(i).color)
    switch i
        case 1
            output='Simulierter Roboter';
        case 2
            output='Hardware in the Loop System';
        case 3
            output='Echter Roboter';
        otherwise
            output='?';
    end
    text(offset_x+dist_x,...
        offset_y+i*dist_y,...
        output);
end


%% add logo
logo_handle=axes('position',[.3  .89  .5  .08])
logo=imread('OvGU_Logo_Fak_INF.jpg');
image(logo);
axis off

%% add the background picture
gh.axHndl=gca;
hold on;
axis equal;
axis off;
color = get(gh.figNumber,'Color');
matrix=imread(scenario.bmp_name,'bmp');
for j=1:3
    matrix_aux(:,:,j) = flipud(matrix(:,:,j));
end
[xmax, ymax] = size(matrix_aux);
gh.axHndl=image(matrix_aux,'Parent',gh.axHndl);
set(gh.axHndl, 'Tag','SimAxes');

cd saves
save gh;
cd ..
end

function my_closereq(src,evnt)
% User-defined close request function
%    selection = questdlg('Close This Figure?',...
%       'Close Request Function',...
%       'Yes','No','Yes');
%    switch selection,
%       case 'Yes',
%         delete(gcf)
%       case 'No'
%       return
%   end

    % stop all timers
    out = timerfind;
    if ~isempty(out)
        stop(out);
        delete(out);
    end
    % close figure
    handle=findobj('Tag','controlfigure');
    if ~isempty(handle)
        delete(handle);
    end
    % close visualisation figure
    delete(gcf)
end