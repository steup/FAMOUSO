function HumanDetCallback(channel_name)

global scenario

global HumanDet
values=[];
[values HumanDet]=get_data(HumanDet,'newest');
if isempty(values)
   return; 
end
if double(values(2))==1
    scenario.HumanDet=1;
    disp('Human detected !!!')
end;
if double(values(2))==0
    scenario.HumanDet=0;
    disp('Human detection allert chanceled')
end;
