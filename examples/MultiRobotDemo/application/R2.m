function new = R2(simrobot,matrix,step)
persistent state;
persistent wait_counter;
% your algorithm starts here
%    getname(simrobot)
% sensor reading
   [aux,num] = readusonic(simrobot,'center',matrix);
% num, the nearest obstacle number, is not used    
 
    %% senden des mittleren Entfernungswertes
    global distance;
    if aux>=255
        aux=255;
    end
    
    if wait_counter<25
        wait_counter=wait_counter+1;
    else
        publishing(distance,[2 aux 118])
        wait_counter=0;
    end
        
    %% lesen der Daten
    global velocity_data
    
    a=[];
    left=0;
    right=0;
    if ~isempty(velocity_data)
        char2int(velocity_data);
        a=find(velocity_data(:,1)==2);
        if ~isempty(a)
            left=uint8TOint8(velocity_data(a(1),2))/100;
            right=uint8TOint8(velocity_data(a(1),3))/100;
            state=0;
            simrobot = setvel(simrobot,[left right]);
            simrobot=setpower(simrobot,1);
        end   
    else
        state=state+1;
        if state<20
             vel=getvel(simrobot);
             left=vel(1);
             right=vel(2);
             simrobot = setvel(simrobot,[left right]);
        else
             simrobot=setpower(simrobot,0);
        end
    end
%    simrobot = setvel(simrobot,[left right]);
% end of your algorithm

new = simrobot;