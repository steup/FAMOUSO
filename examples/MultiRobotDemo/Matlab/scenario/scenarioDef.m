function scenarioDef()
global scenario;

%% Definition of the scenario header
disp(['Definining scenario and saving in ' scenario.filename]);
scenario.startTime=[];
scenario.matrix=[];
scenario.period=0.05;

%% Building the matrix based on a bitmap
%scenario.bmp_name='BMParea.bmp';
scenario.bmp_name='Buero2.bmp';
fprintf('Loading map ...')
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
fprintf('                      [ok]\n');

% -------------------------------------------------------------------------
%% Robot construction site
% -------------------------------------------------------------------------

%% Definition of the sensor parameter
sensors.name='sensor_1';
sensors.position=[5 0];
sensors.axisangle=0;
sensors.scanangle=120;
sensors.range=20;
sensors.resolution=30;
sensors.line=0;

% i=length(scenario.robots)+1;
% sensors(i).name='sensor_2';
% sensors(i).position=[0 6];
% sensors(i).axisangle=90;
% sensors(i).scanangle=20;
% sensors(i).range=20;
% sensors(i).resolution=30;
% sensors(i).line=0;

%% Trigger modi
trigger(1).triggerMode='timer';
trigger(1).period=0.1;
trigger(1).delay=1;
trigger(1).timerHandle=[];
trigger(1).TCPHandle=[];

%Trigger mode
trigger(2).triggerMode='callback';
trigger(2).period=[];
trigger(2).delay=[];
trigger(2).timerHandle=[];
trigger(2).TCPHandle=[];


%% ------> Robot 1
% Robot assambling 1
scenario.robots=simrobot('robot_1',...
    2,...
    180,...
    'robot_T_app',...
    [1 1 0],...
    1,...
    [-2 0 4 0 -2],...
    [3 4 0 -4 -3],...
    sensors, ...
    [100 150], ...
    trigger(1));

%% ------> Robot 2
% Robot assambling 2
i=length(scenario.robots)+1;
scenario.robots(i)=simrobot('robot_2',...
    length(scenario.robots)+1,...
    180,...
    'robot_T_app',...
    [1 1 0],...
    1,...
    [-4 4 4 -4],...
    [4 4 -4 -4],...
    sensors, ...
    [180 150], ...
    trigger(1));
% 
% i=length(scenario.robots)+1;
% scenario.robots(i)=simrobot('robot_3',...
%     length(scenario.robots)+1,...
%     0,...
%     0,...
%     'robot_T_app',...
%     [1 1 0],...
%     1,...
%     [-4 4 4 -4],...
%     [4 4 -4 -4],...
%     sensors, ...
%     [200 150], ...
%     trigger(2));

% i=length(scenario.robots)+1;
% scenario.robots(i)=simrobot('robot_4',...
%     length(scenario.robots)+1,...
%     180,...
%     0,...
%     'robot_T_app',...
%     [1 1 0],...
%     1,...
%     [-4 4 4 -4],...
%     [4 4 -4 -4],...
%     sensors, ...
%     [100+i*20 150], ...
%     trigger(2));
% i=length(scenario.robots)+1;
% 
% scenario.robots(i)=simrobot('robot_5',...
%     length(scenario.robots)+1,...
%     180,...
%     0,...
%     'robot_T_app',...
%     [1 1 0],...
%     1,...
%     [-4 4 4 -4],...
%     [4 4 -4 -4],...
%     sensors, ...
%     [100+i*20 150], ...
%     trigger);

%% Saves
cd saves
save(scenario.filename,'scenario');