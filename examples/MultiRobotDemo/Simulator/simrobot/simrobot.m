function obj = simrobot(varargin)
if ~isempty(varargin)
    aux=max(size(varargin));
else
    aux=0;
end
switch aux
    case 0
        error('Constructor needs parameter for instances !!!')
    case 11
        obj.name = varargin{1};
        obj.number =  varargin{2}; 
        % In degrees, 0 = facing east [°]
        obj.heading = varargin{3};
        % Control file
        obj.af = varargin{4};
        obj.color = varargin{5};
        obj.scale = varargin{6};  
        % Contour data
        obj.xdata = varargin{7}';
        obj.ydata = varargin{8}';
        
        obj.sensors = varargin{9};
        obj.position = varargin{10};
        % Absolute position
        obj.trigger = varargin{11};
        
        obj.power = false;
        obj.crashed = false;
        
        obj.patch=[];
        obj.line=[];

        % Speed of left and right wheel
        obj.velocity = [0 0];
        % Acceleration of left and right wheel
        obj.accel = [0 0];
        
        obj.patchcircle=max(sqrt(obj.xdata.^2+obj.ydata.^2));
        %todo Berechnung einfuegen !!!
        obj.sensorcircle=50;
        
end