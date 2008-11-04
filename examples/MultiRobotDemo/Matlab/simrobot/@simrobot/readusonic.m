function [dists,num] = readusonic(simrobot,sensname,matrix)
% READUSONIC 	reads data from an ultrasonic sensor.
%		See also READLASER.


[tmp s] = size(simrobot.sensors);
i = 1;
while ((i < s) & (~strcmp(simrobot.sensors(i).name,sensname))),
   i = i + 1;
end

aux=[[cos(simrobot.heading*pi/180) sin(simrobot.heading*pi/180)]; ...
    [-sin(simrobot.heading*pi/180) cos(simrobot.heading*pi/180)]];
pos=simrobot.sensors(i).position*simrobot.scale*aux+simrobot.position;

angles = simrobot.heading + simrobot.sensors(i).axisangle - simrobot.sensors(i).scanangle/2: ...
         simrobot.sensors(i).scanangle/2: ...
         simrobot.heading + simrobot.sensors(i).axisangle + simrobot.sensors(i).scanangle/2;
angles = angles*pi/180;

xs=[pos(1) pos(1)+cos(angles)*simrobot.sensors(i).range pos(1)];
ys=[pos(2) pos(2)+sin(angles)*simrobot.sensors(i).range pos(2)];

x_window=[floor(min(xs)) ceil(max(xs))];
y_window=[floor(min(ys)) ceil(max(ys))];

if x_window(1)<1
    x_window(1)=1;
end
if x_window(2)>size(matrix,2)
    x_window(2)=size(matrix,2);
end
if y_window(1)<1
    y_window(1)=1;
end
if y_window(2)>size(matrix,1)
    y_window(2)=size(matrix,1); 
end

%  plot([x_window(1) x_window(2) x_window(2) x_window(1) x_window(1)],...
%       [y_window(1) y_window(1) y_window(2) y_window(2) y_window(1)],'-g');

x=[];
y=[];
dists=999999;
num=1;

[x y] = find(matrix(x_window(1):x_window(2),y_window(1):y_window(2))~=0);
if ~isempty(x)
    x=x+x_window(1)-1;
    y=y+y_window(1)-1;

    for i=1:length(xs)-1
        m(i) = xs(i)*ys(i+1)-ys(i)*xs(i+1);
        n(i) = ys(i)-ys(i+1);
        o(i) = xs(i+1)-xs(i);
    end
    
    in=myinpolygon(x,y,xs,ys);
    if (sum(in)>=1)
         dists =  min(sqrt((x(in)-pos(1)).^2+(y(in)-pos(2)).^2));
    end
end