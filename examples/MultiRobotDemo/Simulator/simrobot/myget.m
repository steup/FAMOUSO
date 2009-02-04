function output = get(a,argument)
%GET Summary of this function goes here
%   Detailed explanation goes here
    output=[];
    switch argument
        case 'xdata'
            output=a.xdata;
        case 'ydata'
            output=a.ydata;
        case 'patchXData' 
            if strcmp(class(a.patch),'double')
                output = get(a.patch,'XData');	% Define the patch
            else
                output=a.patch.x;
            end           
        case 'patchYData'
            if strcmp(class(a.patch),'double')
                output = get(a.patch,'YData');	% Define the patch
            else
                output=a.patch.y;
            end   
        case 'trigger_delay'
            output=a.trigger.delay;
        case 'trigger_period'
            output=a.trigger.period;
         case 'trigger_mode'
            output=a.trigger.mode; 
        case 'crashed'
            output=a.crashed;
        case 'position'
            output=a.position;
        case 'heading'
            output=heading;
        case 'velocity'
            output=velocity;
         case 'name'
            output=velocity;
         case 'number'
            output=number;            
        case {'af','color','scale','patch','power','accel'}
            eval(sprintf('output=a.%s;',argument));
        otherwise
            error('Wrong argument for class simrobot !!!');
    end
end
