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
    
    if max(size(data))>=13
           % global variable
           eval(sprintf('global %s;',a.comment));        
    else
          return
    end
    
    current_subject=[];
%% Searching for valid data
    while(true)
        % contains the current data complete header
        if max(size(data))>=13
            for i=2:9
                aux=dec2hex(double(data(i)));
                if max(size(aux))<2
                    current_subject=[current_subject '0' aux];  
                else
                    current_subject=[current_subject aux];
                end
            end 
            current_length=bitshift(data(10),24)+bitshift(data(11),16)+bitshift(data(12),8)+data(13);
            if max(size(data))>=current_length+13
                 a=inc_index(a);
                values=char(data(14:13+current_length));
                a=set_data(a,0,current_length,values);
                counter=counter+1;
                if max(size(data))>13+current_length
                    b=double(14+current_length);
                    data=data(b:size(data,2));
                else
                    data=[];
                end
            else
                % missing data
                return
            end
        else
            % no complete header
            return
        end
        
        
        
        
%         if (number-k)>12
%             current_subject=[];
% %             [str,maxsize,endian]=computer;
% %             if endian=='L'
% %                 swapbytes(data(2:9));
% %                 swapbytes(data(10:13));
% %             end
%             for i=1:8
%                 aux=dec2hex(double(data(k+i)));
%                 if max(size(aux))<2
%                     current_subject=[current_subject '0' aux];  
%                 else
%                     current_subject=[current_subject aux];
%                 end
%             end
%             %current_length=bitshift(data(k+9),24)+bitshift(data(k+10),16)+bitshift(data(k+11),8)+data(k+12);
%              A=data(k+12)
%             current_length=3
%             k
%             current_length=A
%             % is the data message complete?
%             if ((number-(k+12))>=current_length)
%                 a=inc_index(a);
%                 values=char(data(k+12+1:k+12+current_length));
%                 a=set_data(a,0,current_length,values);
%                 counter=counter+1;
%                 k=k+12+current_length+1;
%             else
%                if counter>0
%                     data=data(k:number);
%                end
%             end
%         else
%         % no complete message header
%             if counter>0
%                 data=data(k:number); 
%             end
%         end
    end