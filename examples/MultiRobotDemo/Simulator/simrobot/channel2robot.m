function channel2robot(channel_name)

global scenario

robotlist=[];

eval(sprintf('global %s;',get_properties(channel_name,'comment')));
aux=get_properties(channel_name,'comment');
eval(sprintf('[values %s]=get_data(%s,''all'');',aux,aux));
% interpretation of the IDs and activation of the according step function

for i=1:size(values,1)
     aux=char2int(values(i,:));
     %% Achtung Achtung hier muss noch nachgebessert werden !!!
     robotid=aux(4);
     id=find(scenario.robotIDs==robotid);
     if isempty(id)
        break
     end
     robotlist=[robotlist id];
     left=aux(2)/120;
     right=aux(3)/120;
     scenario.robots(id)=setvel(scenario.robots(id),[left right]);
end
% size(values,1)
robotlist = unique(robotlist);
for i=1:length(robotlist)
    if ~scenario.robots(i).crashed
        step([],[],robotlist(i),[]);
    end
end
