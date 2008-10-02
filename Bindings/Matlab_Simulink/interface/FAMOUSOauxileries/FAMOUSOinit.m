%% FAMOUSO Channel Settings

% and definition of the channel objects
function FAMOUSOinit(TCPIPsettings)
disp('Starting ECH ...');
%% Defintion of the classes
global input_channel_IDs;
global output_channel_IDs;
input_channel_IDs=struct('name',{},'subject',{});
output_channel_IDs=struct('name',{},'subject',{});

%% Reading the definition file
fprintf('Reading FAMOUSO channels for subscribe / publish ...\n');
[input_def, output_def, maxLength]=ECHconfiguration();
fprintf('\n           subject            comment    type\n------------------------------------------------------\n');

%% input
% initionlisation of the channel and events objects
for i=1:size(input_def,1)
    try
        [subject i_type function_name number comment]=strread(input_def(i,:), '%s %s %s %d %s');
        error=0;
    catch
        disp('Wrong FAMOUSO input channel definition, please check this');
        error=1;
    end
    if error==0
        % TODO: I missed a consitency check of the channel definition here !!!
        % something like this
        % -----------------------------------------------------------------
        if strcmp(i_type,'polling') | strcmp(i_type,'callback')
            if strcmp(i_type,'polling')
                function_name='-';
            end
        else
            error('Unknown TCP\IP configuration - ''polling'' or ''callback'' allowed only');
        end
        % -----------------------------------------------------------------       
        fprintf('subscribe  %s   %s   %s \n', char(subject), char(comment),char(i_type));
        eval(sprintf('global %s;',char(comment)));
        eval(sprintf('%s=input_channel(char(subject), number, char(comment), char(function_name));',char(comment)));
        input_channel_IDs(i).name=char(comment);
        input_channel_IDs(i).subject=char(subject);
    end
end

% Initialisation of each TCP/IP channel
if get_input_channel_info('size')>0
    aux=TCPIP_connection();
    aux=set_properties(aux,'host',TCPIPsettings.host);
    aux=set_properties(aux,'port',TCPIPsettings.port);
    aux=set_properties(aux,'message_length',TCPIPsettings.message_length); 
    for i=1:get_input_channel_info('size')
        channel_name=get_input_channel_info('channel_name',i);
        aux=set_properties(aux,'channel_name',channel_name);
        eval(sprintf('global %s;',channel_name));
        aux=set_properties(aux,'interaction_type',char(i_type));     
        eval(sprintf('%s=set_properties(%s,''connection'',aux);',char(channel_name),char(channel_name)));
    end
end


%% output
% initionlisation of the channel and events objects
for i=1:size(output_def,1)
    try
        [subject comment]=strread(output_def(i,:), '%s %s');
    catch
        disp('Wrong FAMOUSO output channel definition, please check this');
    end
    if error==0
        fprintf('publish    %s   %s\n', char(subject), char(comment));
        eval(sprintf('global %s;',char(comment)));
        eval(sprintf('%s=output_channel(char(subject), char(comment));',char(comment)));
        output_channel_IDs(i).name=char(comment);
        output_channel_IDs(i).subject=char(subject);
    end
end

% Initialisation of each TCP/IP channel
if get_output_channel_info('size')>0
    aux=TCPIP_connection();
    aux=set_properties(aux,'host',TCPIPsettings.host);
    aux=set_properties(aux,'port',TCPIPsettings.port);
    aux=set_properties(aux,'message_length',TCPIPsettings.message_length);
    for i=1:get_output_channel_info('size')
        channel_name=get_output_channel_info('channel_name',i);
        eval(sprintf('global %s;',channel_name));
        aux=set_properties(aux,'interaction_type',char(i_type));
        eval(sprintf('%s=set_properties(%s,''connection'',aux);',char(channel_name),char(channel_name)));
    end
end