function scenarioDef()
global scenario;

%% Definition of the scenario header
disp(['Definining scenario and saving in ' scenario.filename]);
scenario.startTime=[];
scenario.matrix=[];
scenario.period=0.05;
scale=1;

%% Building the matrix based on a bitmap
scenario.bmp_name='reference_img_10_2.bmp';
%scenario.bmp_name='Buero2.bmp';
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
sensors.position=scale*[5 0];
sensors.axisangle=0;
sensors.scanangle=40;
sensors.range=scale*20;
sensors.resolution=20;
sensors.line=0;

i=length(sensors)+1;
sensors(i).name='sensor_2';
sensors(i).position=scale*[0 6];
sensors(i).axisangle=90;
sensors(i).scanangle=20;
sensors(i).range=scale*20;
sensors(i).resolution=20;
sensors(i).line=0;

i=length(sensors)+1;
sensors(i).name='sensor_3';
sensors(i).position=scale*[0 -6];
sensors(i).axisangle=-90;
sensors(i).scanangle=20;
sensors(i).range=scale*20;
sensors(i).resolution=20;
sensors(i).line=0;

%% Trigger modi
% intern
trigger(1).triggerMode='timer';
trigger(1).period=0.1;
trigger(1).delay=2;
trigger(1).timerHandle=[];
trigger(1).TCPHandle=[];

%extern triggered
trigger(2).triggerMode='callback';
trigger(2).period=[];
trigger(2).delay=[];
trigger(2).timerHandle=[];
trigger(2).TCPHandle=[];

RobotIndexOffset=50;

%% ------> Robot 1
% Robot assambling 1
i=1;
scenario.robots=simrobot('robot_1',...
    i,...
    180,...
    'robot_T_app',...
    [1 1 0],...
    1,...
    scale*[-2 0 4 0 -2],...
    scale*[3 4 0 -4 -3],...
    sensors, ...
    [100 150], ...
    trigger(1));

%% ------> Robot 2
% Robot assambling 2
% i=length(scenario.robots)+1;
% scenario.robots(i)=simrobot('robot_2',...
%     length(scenario.robots)+1+RobotIndexOffset,...
%     180,...
%     'robot_T_app',...
%     [0 1 1],...
%     1,...
%     aux*[-2 0 4 0 -2],...
%     aux*[3 4 0 -4 -3],...
%     sensors, ...
%     [200 250], ...
%     trigger(1));

%% ------> Robot 3
% Robot assambling 3
i=length(scenario.robots)+1;
scenario.robots(i)=simrobot('robot_3',...
    64,...
    180,...
    'robot_C_app',...
    [1 1 1],...
    1,...
    scale*[-4 4 4 -4],...
    scale*[4 4 -4 -4],...
    sensors, ...
    [220 150], ...
    trigger(2));
% 
% 
% % ------> Robot 4
% % Robot assambling 4
% i=length(scenario.robots)+1;
% scenario.robots(i)=simrobot('robot_4',...
%     length(scenario.robots)+1,...
%     180,...
%     'robot_T2_app',...
%     [1 1 1],...
%     1,...
%     aux*[-4 4 4 -4],...
%     aux*[4 4 -4 -4],...
%     sensors, ...
%     [350 380], ...
%     trigger(1));

aux=[];
for i=1:length(scenario.robots)
    aux=[aux scenario.robots(i).number];
    if strcmp(scenario.robots(i).trigger.triggerMode,'timer')
        fprintf('Internal robot %d included\n',scenario.robots(i).number)
    else
        fprintf('External robot %d included\n',scenario.robots(i).number)
    end 
end
scenario.robotIDs=aux;

%% Saves
cd saves
save(scenario.filename,'scenario');