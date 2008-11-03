function output = get(a,argument)
%GET Summary of this function goes here
%   Detailed explanation goes here
    switch argument
        case {'name','number','af','xdata','ydata','color','scale','patch','power','crashed','position','heading','velocity','accel'}
            eval(sprintf('output=a.%s;',argument));
        otherwise
            a=output
            %error('Wrong argument for class simrobot !!!')
    end
end
