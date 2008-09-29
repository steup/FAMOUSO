%% Constructor of a input_channel object

% The interaction between incoming messages and Matlab application is
% possible in two ways:
%   - Polling
%   - Callback mechanisms
% Polling is the default definition.

function a = input_channel(varargin)
    switch nargin
        case 0
        % if no input arguments, create a default object
           error('Arguments missed !!!')  
        case 1
        % if single argument of class input_Channel, return it
        % Copy constructor
           if (isa(varargin{1},'input_channel'))
              a = varargin{1};
           else
              error('Wrong argument type')
           end 
        case 4
            a.subject = varargin{1};
            a.number = varargin{2};
            a.comment =  varargin{3};
            a.function_name = varargin{4};
            a.index = 0;
            a.messages(1:a.number)=struct('flag',0,'timestamp', 0, 'length', 0, 'values', 'empty');
            a.connection=TCPIP_connection();
            a = class(a,'input_channel');
        otherwise
           error('Wrong number of input arguments')
    end
end