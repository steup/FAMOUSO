function new = R1(simrobot,matrix,step,id)

% your algorithm starts here

[aux,num] = readusonic(simrobot,'sensor_1',matrix);
% num, the nearest obstacle number, is not used    
 
    %% senden des mittleren Entfernungswertes
    global distance;
    if aux>=255
        aux=255;
    end
  
    publishing(distance,[aux 118 id])
   
    
    %% lesen der Daten
    global velocity_data
   
    a=[];
    left=0;
    right=0;
    if ~isempty(velocity_data)
        char2int(velocity_data);
        a=find(velocity_data(:,3)==id);
        if ~isempty(a)
            left=uint8TOint8(velocity_data(a(1),1))/100;
            right=uint8TOint8(velocity_data(a(1),2))/100;
            state=0;
            simrobot = setvel(simrobot,[left right]);
            simrobot=setpower(simrobot,1);
        end   
    end

new = simrobot;