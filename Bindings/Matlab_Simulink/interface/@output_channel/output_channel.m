 function a = output_channel(varargin)
    switch nargin
        case 0
        % if no input arguments, create a default object
           error('Arguments missed !!!')  
        case 1
        % if single argument of class input_Channel, return it
        % Copy constructor
           if (isa(varargin{1},'output_channel'))
              a = varargin{1};
           else
                a.subject = varargin{1};
                a.comment = 'empty';
                a = class(a,'output_channel');
           end 
        case 2
            a.subject = varargin{1};
            a.comment = varargin{2};
            a.connection = TCPIP_connection();
            a = class(a,'output_channel');
        otherwise
           error('Wrong number of input arguments')
    end
end