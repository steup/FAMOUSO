function new = robot_P_app(simrobot, id, matrix)
global scenario
persistent handle
% your algorithm starts here

% sensor reading
[center,num] = readusonic(simrobot,id,'sensor_1',matrix);
[left,num] = readusonic(simrobot,id,'sensor_2',matrix);
[right,num] = readusonic(simrobot,id,'sensor_3',matrix);
[dist,num] = readusonic(simrobot,id,'sensor_40',matrix);

streuung=3;
control_area=ones(2*streuung+1,2*streuung+1);
aux_matrix=filter2(control_area,matrix);

alpha=simrobot.heading-10:1:simrobot.heading+10;
alpha=simrobot.heading;
alpha=pi*alpha/180;

[x_aux,y_aux]=find(aux_matrix);
% figure(4)
% hold on
% plot(x_aux,y_aux,'.k')

num=floor(center);
x=cos(alpha)*num;
y=sin(alpha)*num;
pos=round([x_aux-x,y_aux-y]);
ind=find(pos(:,1)>0 & pos(:,1)<=size(aux_matrix,1) & pos(:,2)>0 & pos(:,2)<=size(aux_matrix,2));
center_aux=zeros(size(scenario.matrix));
for i=1:length(ind)
    center_aux(pos(ind(i),1),pos(ind(i),2))=1;
end
% [x,y]=find(center_aux);
% plot(x,y,'.y')

alpha_left=alpha+pi/2;
num=floor(left);
x=cos(alpha_left)*num;
y=sin(alpha_left)*num;
pos=round([x_aux-x,y_aux-y]);
ind=find(pos(:,1)>0 & pos(:,1)<=size(aux_matrix,1) & pos(:,2)>0 & pos(:,2)<=size(aux_matrix,2));
left_aux=zeros(size(scenario.matrix));
for i=1:length(ind)
    left_aux(pos(ind(i),1),pos(ind(i),2))=1;
end
% [x,y]=find(left_aux);
% plot(x,y,'.r')

alpha_right=alpha-pi/2;
num=floor(right);
x=cos(alpha_right)*num;
y=sin(alpha_right)*num;
pos=round([x_aux-x,y_aux-y]);
ind=find(pos(:,1)>0 & pos(:,1)<=size(aux_matrix,1) & pos(:,2)>0 & pos(:,2)<=size(aux_matrix,2));
right_aux=zeros(size(scenario.matrix));
for i=1:length(ind)
    right_aux(pos(ind(i),1),pos(ind(i),2))=1;
end
% [x,y]=find(right_aux);
% plot(x,y,'.b')

[x,y]=find(right_aux & left_aux & center_aux);

if ~isempty(handle)
   try
     delete(handle);
   catch
       
   end
end
% xlim('manual')
% ylim('manual')
gca=findobj('Tag','SimAxes');
handle=plot(gca,x,y,'.m','MarkerSize',10);

if dist<15
    simrobot = setvel(simrobot,[-0.3 0.3]);  % turn left
else
    simrobot = setvel(simrobot,[0.6 0.6]); % go straight on
end

new = simrobot;
end
