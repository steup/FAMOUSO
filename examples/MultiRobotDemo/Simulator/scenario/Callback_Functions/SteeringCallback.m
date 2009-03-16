function steeringCallback(channel_name)

global scenario

robotlist=[];

global Steering
values=[];
[values Position]=get_data(Steering,'all');
if isempty(values)
   return; 
end

if scenario.HumanDet==1
   return; 
end

for i=1:size(values,1)
    %% Achtung Achtung hier muss noch nachgebessert werden !!!
    robotid=double(typecast(int8(aux(1)), 'int8'));
    id=find(scenario.robotIDs==robotid);
    if robotid==255
        id=255; 
    end
    if isempty(id)
        break
    end
    robotlist=[robotlist id];
    k=1/10;
    left=double(typecast(int8(aux(3)), 'int8'))*k;
    right=double(typecast(int8(aux(4)), 'int8'))*k;
    if id~=255
        scenario.robots(id)=setvel(scenario.robots(id),[left right]);
        for i=1:length(scenario.robots)
            scenario.robots(i).panelControled=false;
        end
        scenario.robots(id).panelControled=true;
    else
        for i=1:length(scenario.robots)
        scenario.robots(i)=setvel(scenario.robots(i),[left right]);
        scenario.robots(i).panelControled=false;
        end
        scenario.robots(id).panelControled=true;
    end
end

robotlist = unique(robotlist);
for i=1:length(robotlist)
    if ~scenario.robots(i).crashed
        step([],[],robotlist(i),[]);
    end
end