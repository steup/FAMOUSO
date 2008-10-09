function new = R1(simrobot,matrix,step)

% your algorithm starts here
%    getname(simrobot)
% sensor reading
    aux=zeros(3,1);  
   [aux(1),num] = readusonic(simrobot,'left',matrix);
   [aux(2),num] = readusonic(simrobot,'center',matrix);
   [aux(3),num] = readusonic(simrobot,'right',matrix);   
% num, the nearest obstacle number, is not used    
 
    %% senden des mittleren Entfernungswertes
    global distance;
    if aux(2)>=255
        aux(2)=255;
    end
    publishing(distance,[1 aux(2)]);
    
    %% lesen der Daten
    global velocity_data
    
    a=[];
    if ~isempty(velocity_data)
        char2int(velocity_data);
        a=find(velocity_data(:,1)==1);
        if ~isempty(a)
            left=uint8TOint8(velocity_data(a,2))/100;
            right=uint8TOint8(velocity_data(a,3))/100;
            simrobot = setvel(simrobot,[left right]);
        else
            simrobot = setvel(simrobot,[0 0]);
        end
    else
        simrobot = setvel(simrobot,[0 0]);
    end
   
% end of your algorithm

new = simrobot;