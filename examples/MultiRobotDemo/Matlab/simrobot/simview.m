function gh=simview(scenario);

%% close all related windows
h = findobj('Tag','SimWindow');
delete(h);

%% Create a new simulator figure
gh.figNumber = figure;

ssize = get(0,'ScreenSize');
set(gh.figNumber, ...
    'Position',[5 ssize(4)*0.1 ssize(3)/2 ssize(4)*0.8],...
    'NumberTitle','off',...
    'Name','Simulation Window',...
    'RendererMode','manual',...
    'Renderer','Painters',...
    'Backingstore','off',...
    'Tag','SimWindow');

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