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
function publishing(a,data)
%% Checking connection
    if ~get_properties(a.connection,'tcp_active');
       error('Error - No connection for publishing \n');
    end

%% Assambling of the header + subject + length + data
% for publish messages the header is defined to 'ECH_REQ_PUBLISH'=0x52='R'
    if size(data,2)>0
        if max(size(data))<9
%             header='52';
%             output=hex2dec(header);
%             aux=get_properties(a,'subject');
%             % distribution of the channel to 8 byte
%             for i=1:8
%                 result=hex2dec(aux(2*i-1:2*i));
%                 output=[output char(result)];
%             end
            output=['R' a.comment];
            output=[output char(0) char(0) char(0)];
            output=[output char(max(size(data)))];
            %data=int8(data);
            for i=1:max(size(data))
            %     if (data(i)>126 || data(i)<-127)
            %         disp('Wertebereich von int8 verlassen !!!')
            %         data(i)
            %     else
                    if data(i)>=0
                        output=[output char(data(i))];
                    else
                        output=[output char(256+data(i))];        
                    end
            %     end
            end
            %% Sending
            TCPIP_write(a.connection, output);
        else
            disp('Size of data is limited to 8 Byte !');
        end
    else
        disp('Data is empty !!!');
    end

