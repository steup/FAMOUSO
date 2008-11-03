function a = set( a, argument, value )
%SET Summary of this function goes here
%   Detailed explanation goes here
    switch argument
        case {'name','number','af','color','scale','patch','power','crashed','position','heading','velocity','accel'}
            eval(sprintf('a.%s=value;',argument));
        otherwise
            error('Wrong argument for class simrobot !!!')
    end
end
