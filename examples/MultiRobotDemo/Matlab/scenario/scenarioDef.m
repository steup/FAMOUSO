function scenario=scenarioDef()

%% Definition of the scenario header
scenario.fileName='scenarioFile'
scenario.startTime=[];
scenario.matrix=[];
scenario.period=0.05;
scenario.visualise=1;

%% Definition of the sensor parameter
disp('Building one robot manualy')

sensors.name='sensor_1';
sensors.position=[5 0];
sensors.axisangle=0;
sensors.scanangle=120;
sensors.range=20;
sensors.resolution=30;
sensors.line=0;

%% Robot assambling
scenario.robots=simrobot('robot_1',...
    2,...
    180,...
    1,...
    'robot_1_app',...
    [1 1 0],...
    1,...
    [-2 0 4 0 -2],...
    [3 4 0 -4 -3],...
    sensors, ...
    [150 150]);

%% Building the matrix based on a bitmap
scenario.bmp_name='BMParea.bmp';
%scenario.bmp_name='BueroLowResolution.bmp';
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
fprintf('     finished \n');

%% Saves
cd saves
save(scenario.fileName,'scenario');
cd ..