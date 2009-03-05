% Otto-von-Guericke University Magdeburg
% Department for distributed system
% Sebastian Zug
%
function start()
%% Start the simrobot simulation environement 
% and a visualisation tool according to the configuration 
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
clear global scenario;
warning off all
delete(timerfind);
delete(findobj('name','control'));
home;
setpaths;
disp('Lets go!');

%% determine scenario source
global scenario;
% select
%scenario.source='file';
% or
scenario.source='new';
% and the according file
scenario.filename='scenarioFile.mat';


%% FAMOUSO integration?
scenario.FAMOUSO=0;

%% Start respective environement
% select for simulation only
scenario.mode='sim';
% % or 
scenario.mode='vis+sim';
% % for simulation and visualisation
GUIcontrol();