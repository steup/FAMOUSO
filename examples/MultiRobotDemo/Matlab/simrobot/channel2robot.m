function channel2robot(channel_name)

global scenario

robotlist=[];

eval(sprintf('global %s;',get_properties(channel_name,'comment')));
eval(sprintf('values=get_data(%s,''all'');',get_properties(channel_name,'comment')));
% interpretation of the IDs and activation of the according step function
for i=1:size(values,1)
   aux=char2int(values(i,:));
   id=aux(3);
   robotlist=[robotlist id];
   left=uint8TOint8(aux(1))/100;
   right=uint8TOint8(aux(2))/100;
   scenario.robots(id)=setvel(scenario.robots(id),[left right]);
end

robotlist = unique(robotlist);
for i=1:length(robotlist)
    if ~get(scenario.robots(i),'crashed')
        step([],[],robotlist(i));
    end
end

function aux=uint8TOint8(a)
if a<=255 && a>127
    aux=a-256;
else
    if a<0
        error('wrong input!');
    else
        aux=a;
    end
end