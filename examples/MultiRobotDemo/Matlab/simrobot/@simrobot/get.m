function output = get(a,argument)
%GET Summary of this function goes here
%   Detailed explanation goes here
    output=[];
    switch argument
        case 'xdata'
            output=a.xdata;
        case 'ydata'
            output=a.ydata;
        case {'name','number','af','color','scale','patch','power','crashed','position','heading','velocity','accel'}
            eval(sprintf('output=a.%s;',argument));
        otherwise
            error('Wrong argument for class simrobot !!!');
    end
end
