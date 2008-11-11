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
clear scenario;
home;
setpaths;
disp('Lets go!');

%% Opening control GUI
aux=control;

%% determine scenario source
global scenario;
% select
%scenario.source='file';
% or
scenario.source='new';
% and the according file
scenario.filename='scenarioFile.mat';

%% Start respective environement
% select for simulation only
scenario.mode='sim';
% or 
scenario.mode='vis+sim';
% for simulation and visualisation

control('go_Callback',aux);