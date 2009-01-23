function channel2robot(channel_name)

global scenario

robotlist=[];

eval(sprintf('global %s;',get_properties(channel_name,'comment')));
eval(sprintf('values=get_data(%s,''all'');',get_properties(channel_name,'comment')));
% interpretation of the IDs and activation of the according step function
for i=1:size(values,1)
     aux=char2int(values(i,:));
     id=aux(3)+1;
     robotlist=[robotlist id];
%      left=uint8TOint8(aux(1))/120;
%      right=uint8TOint8(aux(2))/120;
     left=aux(1)/120;
     right=aux(2)/120;
     scenario.robots(id)=setvel(scenario.robots(id),[left right]);
end

robotlist = unique(robotlist);
for i=1:length(robotlist)
    if ~scenario.robots(i).crashed
        step([],[],robotlist(i),[]);
    end
end
