function scenarioDef()
global scenario;

%% Definition of the scenario header
disp(['Definining scenario and saving in ' scenario.filename]);
scenario.startTime=[];
scenario.matrix=[];
scenario.period=0.05;

%% Building the matrix based on a bitmap
scenario.bmp_name='BMParea.bmp';
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

%% ------> Robot 1
% Trigger mode
trigger.mode='timer';
trigger.period=0.1;
trigger.delay=1;
trigger.timerHandle=[];

% Robot assambling 1
scenario.robots=simrobot('robot_1',...
    2,...
    180,...
    0,...
    'robot_1_app',...
    [1 1 0],...
    1,...
    [-2 0 4 0 -2],...
    [3 4 0 -4 -3],...
    sensors, ...
    [150 150], ...
    trigger);

%% ------> Robot 2
i=length(scenario.robots)+1;
sensors(i).name='sensor_2';
sensors(i).position=[0 6];
sensors(i).axisangle=90;
sensors(i).scanangle=20;
sensors(i).range=20;
sensors(i).resolution=30;
sensors(i).line=0;


%Trigger mode
trigger.mode='timer';
trigger.period=0.1;
trigger.delay=5;
trigger.timerHandle=[];

% Robot assambling 2
i=length(scenario.robots)+1;
scenario.robots(i)=simrobot('robot_2',...
    2,...
    180,...
    0,...
    'robot_1_app',...
    [1 1 0],...
    1,...
    [-4 4 4 -4],...
    [4 4 -4 -4],...
    sensors, ...
    [150 150], ...
    trigger);

%     [-2 0 4 0 -2],...
%     [3 4 0 -4 -3],...
%% Saves
cd saves
save(scenario.filename,'scenario');