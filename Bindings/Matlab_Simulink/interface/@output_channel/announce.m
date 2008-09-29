%% 
%   tcpobj         TCP/IP handle of the pnet communictation toolbox
%   subject_index  representing the first Byte of the 8 Byte subject 
%                  identifier (hex value !!!)
%   data           vector of n Bytes
%
% Publishes data via FAMOUSO and TCP/IP
%
% Format of the message
% | 1 byte | 8 byte            | 4 byte  | n byte
% | header | subject           | length  | payload
%
function a=announce(a)
%% Assambling of the header + subject + length + data
% for publish messages the header is defined to 'ECH_REQ_ANNOUNCE'=0x56
    if ~get_properties(a.connection,'tcp_active');
       fprintf('Error - No connection for subscribtion \n');
       return;
    else
        header='56';
        output=hex2dec(header);
        aux=a.subject;
        % distribution of the channel to 8 byte
        for i=1:8
            result=hex2dec(aux(2*i-1:2*i));
            output=[output char(result)];
        end
        %% Sending
        TCPIP_write(a.connection, output);
        fprintf('Announcement for channel %s\n', a.subject);
    end