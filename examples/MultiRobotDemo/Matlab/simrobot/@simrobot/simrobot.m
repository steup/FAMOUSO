classdef simrobot
    properties(SetAccess = private)
        patch = [];
        line = [];    
    end
    properties
        name = '';
        number = [];
        af = '';
        color = [];
        scale = [];
        
        position = [];
        heading = [];
        velocity = [];
        accel = [];
        sensors = struct('name',[],'position',[],'axisangle',[],'scanangle',[],...
            'range',[],'resolution',[]);
        history = [];
        userdata = [];
        power = [];
        crashed = [];
        xdata = [];
        ydata = [];
        trigger = struct('tiggerMode',[],'period',[],'delay',[],'timerHandle',[],...
            'TCPHandle',[]);
    end
    methods
        function obj = simrobot(varargin)
            if ~isempty(varargin)
                aux=max(size(varargin));
            else
                aux=0;
            end
            switch aux
                case 0
                    error('Constructor needs parameter for instances !!!')
                case 1
                    if isa(varargin{1},'simrobot')
                        obj = varargin{1};
                    else
                        error('Incomplete definition passed to simrobot constructor');
                    end
                case 12
                    obj.name = varargin{1};
                    obj.number =  varargin{2};
                    % store the path to control algorithm file
                    obj.af = varargin{5};
                    obj.color = varargin{6};
                    obj.scale = varargin{7};
                    obj.history = [];
                    obj.userdata = [];
                    % Power switch
                    obj.power = varargin{4};
                    obj.crashed = 0;
                    
                    obj.patch = [];
                    obj.xdata = varargin{8}';
                    obj.ydata = varargin{9}';
                    
                    obj.line = [];
                    
                    % Absolute position
                    obj.position = varargin{11};
                    % In degrees, 0 = facing east [°]
                    obj.heading = varargin{3};
                    % Speed of left and right wheel
                    obj.velocity = [0 0];
                    % Acceleration of left and right wheel
                    obj.accel = [0 0];
                    obj.sensors = varargin{10};
                    obj.trigger = varargin{12};
                end
            end
        end
    end
