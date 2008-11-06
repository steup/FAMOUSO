function output = get(a,argument)
%GET Summary of this function goes here
%   Detailed explanation goes here
    output=[];
    switch argument
        case 'xdata'
            output=a.xdata;
        case 'ydata'
            output=a.ydata;
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
