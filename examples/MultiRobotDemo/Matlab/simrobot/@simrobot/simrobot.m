function r = simrobot(varargin)                  
% SIMROBOT		Contructor of 'simrobot' object

%	Please try to pass correct parameters, they are not (mostly) checked
%	It is assumed that the robot is heading east (=> need such data for
%	patch definition)

if ~isempty(varargin)
    aux=max(size(varargin));
else
    aux=0;
end
switch aux
    case 0
        r.name = '';
        r.number = [];
        r.af = '';
        r.color = [];
        r.scale = [];
        
        r.patch = [];
        r.line=[];
        
        r.position = [];
        r.heading = [];
        r.velocity = [];
        r.accel = [];
        r.sensors = struct('name',[],'position',[],'axisangle',[],'scanangle',[],...
            'range',[],'resolution',[]);
        r.history = [];
        r.userdata = [];
        r.power = [];
        r.crashed = [];
        r.xdata = [];
        r.ydata = [];
        r.trigger = [];
        r = class(r, 'simrobot');
        disp('Empty robot defined !!!')
    case 1
        if isa(varargin{1},'simrobot')
            r = varargin{1};
        else
            error('Incomplete definition passed to simrobot constructor');
        end
    case 12
        r.name = varargin{1};	
        r.number =  varargin{2};
        % store the path to control algorithm file
        r.af = varargin{5};							
        r.color = varargin{6};
        r.scale = varargin{7};   
        r.history = [];
        r.userdata = [];	
        % Power switch
        r.power = varargin{4};					
        r.crashed = 0;
        
        r.patch = [];
        r.xdata = varargin{8}';                         
        r.ydata = varargin{9}';
        
        r.line = [];
        
        % Absolute position
        r.position = varargin{11};	
        % In degrees, 0 = facing east [�]					
        r.heading = varargin{3};	
        % Speed of left and right wheel
        r.velocity = [0 0];	
        % Acceleration of left and right wheel
        r.accel = [0 0];								
        r.sensors = struct('name',[],'position',[],'axisangle',[],'scanangle',[],...
                                'range',[],'resolution',[],'line',[]);
        r.sensors = varargin{10};                    
                       
        r.trigger = varargin{12};
        r = class(r, 'simrobot');				% Go !!   
end