%% Sends a Subsribe Message to the TCP/IP Broker
% Format of the message
% | 1 byte | 8 byte            |
% | header | subject           |
%

function a=subscribe(a)
    %% Check connection
    if ~get_properties(a.connection,'tcp_active');
       fprintf('Error - No connection for subscribtion \n');
       return; 
    else
        %% Assambling of the header + subject
        % for subscribe messages the header is defined to 'ECH_REQ_SUBSCRIBE'=0x50 
        header='50';
        output=hex2dec(header);
        aux=a.subject;
        % distribution of the channel to 8 byte
        for i=1:8
            result=hex2dec(aux(2*i-1:2*i));
            output=[output char(result)];
        end
        %% Sending
        a.connection=TCPIP_write(a.connection, output);
        fprintf('Subscribtion for channel %s\n', get_properties(a,'subject'));
    end