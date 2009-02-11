function Callback_TCP_BytesAvailableFcn(obj,event,tcpobj)

%% obtain the correct global object based on the channel name inside tcpobj
    if get_input_channel_info('size')>0
        for i=1:get_input_channel_info('size')
            channel_name=get_input_channel_info('channel_name',i);
            if strcmp(channel_name, get_properties(tcpobj,'channel_name'))
                % get global channel object
                eval(sprintf('global %s;',channel_name));
                break;
            end
        end
    end

%% read all messages
    persistent data;
    % add the residue of the last cycle to the new values
    input=TCPIP_read(eval(sprintf('get_properties(%s,''connection'');',channel_name)));
%     size(input)
    if isempty(data)
        data=input;
    else
        data=[data; input];
    end
%     size(data)
    number=max(size(data));
    if number == 0
        return
    end
    k=1;

    %% Searching for valid data
    while(true)
        % contains the current data complete header?
%         fprintf('#')
        if (number-k)>12
            current_subject=[];
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
                eval(sprintf('%s=inc_index(%s);',channel_name,channel_name));
                % a=inc_index(a);
                % Copy the data into the global variable
                t=clock;
                values=char(data(k+12+1:k+12+current_length));
                eval(sprintf('%s=set_data(%s,t(6),current_length,values);',channel_name,channel_name));

                % k points on the first value of the next message
                k=k+12+current_length+1;
                % no valid data any more
                if number<k
                    data=data(k:size(data,2));
                    break;
                end
            else
                % no complete message data
                break;
            end
        else
            % no complete message header
            break;
        end
        % Reading the rest
    end
%     fprintf('\n')
    %% Call of the corresponding function
    function_name=eval(sprintf('get_properties(%s,''function_name'');',channel_name));
    if ~strcmp(function_name,'-')
        [pathstr, name, ext, versn] = fileparts(function_name);
        try
            eval(sprintf('%s(%s)',name,channel_name));
        catch
            error(['Wrong callback function for FAMOUSO event' function_name])
        end
    end
end
