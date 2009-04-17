%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%
%% Copyright (c) 2008, 2009 Sebastian Zug <zug@ivs.cs.uni-magdeburg.de>
%% All rights reserved.
%%
%%    Redistribution and use in source and binary forms, with or without
%%    modification, are permitted provided that the following conditions
%%    are met:
%%
%%    * Redistributions of source code must retain the above copyright
%%      notice, this list of conditions and the following disclaimer.
%%
%%    * Redistributions in binary form must reproduce the above copyright
%%      notice, this list of conditions and the following disclaimer in
%%      the documentation and/or other materials provided with the
%%      distribution.
%%
%%    * Neither the name of the copyright holders nor the names of
%%      contributors may be used to endorse or promote products derived
%%      from this software without specific prior written permission.
%%
%%
%%    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
%%    IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
%%    TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
%%    PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
%%    OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
%%    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
%%    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
%%    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
%%    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
%%    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
%%    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
%%
%%
%% $Id$
%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

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


%% add the background picture
gh.oldaxHndl=gca;
hold on;
axis equal;
axis off;
color = get(gh.figNumber,'Color');
matrix=imread(scenario.bmp_name,'bmp');
for j=1:3
    matrix_aux(:,:,j) = flipud(matrix(:,:,j));
end
[xmax, ymax] = size(matrix_aux);
gh.axHndl=image(matrix_aux,'Parent',gh.oldaxHndl);
set(gh.axHndl, 'Tag','SimAxes');

delete(findobj('type','line'));
delete(findobj('type','patch'));

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
logo_handle=axes('position',[.3  .89  .5  .08]);
logo=imread('OvGU_Logo_Fak_INF.jpg');
image(logo);
axis off

set(gcf,'CurrentAxes',gh.oldaxHndl);
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