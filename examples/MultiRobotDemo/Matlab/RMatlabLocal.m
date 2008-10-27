function new = R2(simrobot,matrix,step)
global distance;
% your algorithm starts here
%    getname(simrobot)
% sensor reading
    aux=zeros(3,1);  
    
   [aux,num] = readusonic(simrobot,'center',matrix);
% num, the nearest obstacle number, is not used    
 
   % publishing measurement values
   if aux>=255
        aux=255;
   end
   publishing(distance,[2 aux 118]);
   
   % reading
   
    threshold=50;
  
    if aux>50
            simrobot = setvel(simrobot,[0.7 0.7]);
    else
            simrobot = setvel(simrobot,[0.2 -0.2]);
    end
% end of your algorithm

new = simrobot;