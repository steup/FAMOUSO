function sensors = sensor_pool(scale)

sensors.name='sensor_1';
%sensors.position=scale*[4 0];
sensors.position=[0 0];
sensors.axisangle=5;
sensors.scanangle=50;
sensors.range=scale*30;
sensors.resolution=1;
sensors.line=0;

i=length(sensors)+1;
sensors(i).name='sensor_2';
% sensors(i).position=scale*[2 2];
sensors(i).position=[0 0];
sensors(i).axisangle=55;
sensors(i).scanangle=100;
sensors(i).range=scale*20;
sensors(i).resolution=10;
sensors(i).line=0;

i=length(sensors)+1;
sensors(i).name='sensor_3';
% sensors(i).position=scale*[2 -2];
sensors(i).position=[0 0];
sensors(i).axisangle=-50;
sensors(i).scanangle=110;
sensors(i).range=scale*20;
sensors(i).resolution=10;
sensors(i).line=0;

%% Sensor f�r die Simulierten Roboter
i=length(sensors)+1;
sensors(i).name='sensor_4';
% sensors(i).position=scale*[3 -7];
sensors(i).position=[0 0];
sensors(i).axisangle=-50;
sensors(i).scanangle=110;
sensors(i).range=scale*20;
sensors(i).resolution=10;
sensors(i).line=0;

%% Sensoren f�r die Positionssch�tzung ...
i=length(sensors)+1;
sensors(i).name='sensor_1';
sensors(i).position=scale*[4 0];
sensors(i).axisangle=5;
sensors(i).scanangle=5;
sensors(i).range=scale*30;
sensors(i).resolution=1;
sensors(i).line=0;

i=length(sensors)+1;
sensors(i).name='sensor_2';
sensors(i).position=scale*[2 2];
sensors(i).axisangle=90;
sensors(i).scanangle=5;
sensors(i).range=scale*30;
sensors(i).resolution=5;
sensors(i).line=0;

i=length(sensors)+1;
sensors(i).name='sensor_3';
sensors(i).position=scale*[2 -2];
sensors(i).axisangle=-90;
sensors(i).scanangle=5;
sensors(i).range=scale*30;
sensors(i).resolution=5;
sensors(i).line=0;

%% Sensor für reale Roboter
i=length(sensors)+1;
sensors(i).name='sensor_1';
sensors(i).position=scale*[5 0];
sensors(i).axisangle=0;
sensors(i).scanangle=110;
sensors(i).range=scale*20;
sensors(i).resolution=10;
sensors(i).line=0;