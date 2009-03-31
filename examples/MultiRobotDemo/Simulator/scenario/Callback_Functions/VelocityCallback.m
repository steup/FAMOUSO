function VelocityCallback(channel_name)

global scenario

robotlist=[];

%% Variable L�sung ...
% aux=get_properties(channel_name,'comment');
% eval(sprintf('global %s;',aux));
% eval(sprintf('[values %s]=get_data(%s,''all'');',aux,aux));

%% Spezifische Implementierung
global Velocity
[values Velocity]=get_data(Velocity,'all');

% interpretation of the IDs and activation of the according step function
for i=1:size(values,1)
    
     aux=char2int(values(i,:));    
     %% Achtung Achtung hier muss noch nachgebessert werden !!!
     robotid=aux(2);
     id=find(scenario.robotIDs==robotid);
     if isempty(id)
        break
     end
     robotlist=[robotlist id];
     k=1/10;
         left=double(typecast(int8(aux(3)), 'int8'))*k;
         right=double(typecast(int8(aux(4)), 'int8'))*k;
         scenario.robots(id)=setvel(scenario.robots(id),[left right]);

end
% double(values)
% size(values,1)
robotlist = unique(robotlist);
for i=1:length(robotlist)
    if ~scenario.robots(i).crashed
%         disp('just in front of step')
        step([],[],robotlist(i),[]);
    end
end