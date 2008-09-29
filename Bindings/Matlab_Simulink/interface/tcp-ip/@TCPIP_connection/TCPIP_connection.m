function a = TCPIP_connection(varargin)
    switch nargin
        case 0
        % if no input arguments, create a default object
            a.tcpobj = [];
            a.interaction_type = [];
            a.tcp_active=0;
            a.host=[];
            a.port=[];
            a.message_length=[];
            a.channel_name=[];
            a = class(a,'TCPIP_connection');
        case 1
        % if single argument of class input_Channel, return it
        % Copy constructor
           if (isa(varargin{1},'TCPIP'))
             	a = varargin{1};
           else
                error('Wrong parameter for TCPIP constructor')
           end 
        case 3
            a.tcpobj = [];
            a.interaction_type = [];
            a.tcp_active=0;
            a.channel_name=[];
            a.host=varargin{1};
            a.port=varargin{2};
            a.message_length=varargin{3};
            a = class(a,'TCPIP_connection');
        otherwise
           error('Wrong number of input arguments')
    end
end


%package='pnet';