%% Sends a Subsribe Message to the TCP/IP Broker
% Format of the message
% | 1 byte | 8 byte            |
% | header | subject           |
%

function [counter,a]=get_event(a) 
counter=0;
%% Checking connection
    if ~get_properties(a.connection,'tcp_active');
       error('Error - No connection for get_event() \n');
    end
    
%% Defined channel or all?
    if ~(isa(a,'input_channel'))
        disp('Wrong parameter to get_event(), type input_channel expected !!!')
        return
    end

%% read all messages
    persistent data;
    % add the residue of the last cycle to the new values
    input=TCPIP_read(a.connection);
    data=[data input];
    number=size(data,2);
    if number == 0
        return
    end    
    k=1;  

%% Searching for valid data
    while(true)
        % contains the current data complete header
        if (number-k)>12
            current_subject=[];
%             [str,maxsize,endian]=computer;
%             if endian=='L'
%                 swapbytes(data(2:9));
%                 swapbytes(data(10:13));
%             end
            for i=1:8
                aux=dec2hex(double(data(k+i)));
                if max(size(aux))<2
                    current_subject=[current_subject '0' aux];  
                else
                    current_subject=[current_subject aux]; 
                end
            end
            current_length=data(k+9)*256^3+data(k+10)*256^2+data(k+11)*256+data(k+12);
            % is the data message complete?
            if ((number-(k+12))>=current_length)
                % global variable
                eval(sprintf('global %s;',a.comment));
  %              if  strcmp(current_subject,aux)
                        % check the index for overflow and increment them
%                         eval(sprintf('%s=inc_index(%s);',a.comment,a.comment));
%                         % Copy the data into the global variable
%                         t=clock;
%                         values=char(data(k+12+1:k+12+current_length))
%                         eval(sprintf('%s=set_data(%s,t(6),current_length,values);',a.comment,a.comment));
%                         a
                            a=inc_index(a);
                            % Copy the data into the global variable
                            t=clock;
                            values=char(data(k+12+1:k+12+current_length));
                            a=set_data(a,t(6),current_length,values);
                        counter=counter+1;
    %            end                 
                % k points on the first value of the next message
                k=k+12+current_length+1;
                % no outher data more
                if number<k
                    data=data(k:size(data,2));              
                    break;
                end
            else
            % no complete message data    
            end
        else
        % no complete message header
        end
        % Reading the rest
    end