function new = R1(simrobot,matrix,step)
global distance;
% your algorithm starts here
%    getname(simrobot)
% sensor reading
    aux=zeros(3,1);  
   [aux(1),num] = readusonic(simrobot,'left',matrix);
   [aux(2),num] = readusonic(simrobot,'center',matrix);
   [aux(3),num] = readusonic(simrobot,'right',matrix);   
% num, the nearest obstacle number, is not used    
 
    threshold=50;
    dist=0;
    for i=1:3
       if aux(i)<threshold 
            dist=dist+power(2,i-1);
       end
    end

 %  publishing(distance,[100+dist 100 101 102 0 0 0 0]);
    
    switch dist
        case 0  % alles aus
            simrobot = setvel(simrobot,[0.7 0.7]);
            
        case 1  % left only 
            simrobot = setvel(simrobot,[-0.2 0.2]);              
        case 3  % left and center
            simrobot = setvel(simrobot,[-0.2 0.2]);
            
        case 2   % center only
            simrobot = setvel(simrobot,[0 0.2]); 
              
        case 4  % right only
            simrobot = setvel(simrobot,[0.2 -0.2]);  
        case 6   % center and right 
            simrobot = setvel(simrobot,[0.2 -0.2]);            

        case 5   % center and left 
            simrobot = setvel(simrobot,[0.2 -0.2]);
        case 7   % all
            simrobot = setvel(simrobot,[0.2 -0.2]);
    end
    aux
    dist
% end of your algorithm

new = simrobot;