clear variables;
delete(timerfind);
%clear classes;
%clear functions;
home;
setpaths;
disp('Lets go!');

global scenario;

scenario.startTime=[];
scenario.matrix=[];
scenario.bmp_name='BMParea.bmp';
%scenario.bmp_name='BueroLowResolution.bmp';
scenario.period=0.05;
scenario.visualise=1;

disp('Building one robot manualy')
sensors.name='sensor_1';
sensors.position=[5 0];
sensors.axisangle=0;
sensors.scanangle=120;
sensors.range=20;
sensors.resolution=30;
sensors.line=0;

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

cd saves
save('scenario','scenario');
cd ..
disp('Starting simulation environment ...');
if scenario.visualise==1
    global gh;
    gh=simview(scenario);
end
simviewcb('init');