%% Start TCP/IP connections
function FAMOUSOsubscribeAll()
if get_input_channel_info('size')>0
    for i=1:get_input_channel_info('size')
        channel_name=get_input_channel_info('channel_name',i);
        eval(sprintf('global %s;',channel_name));
        eval(sprintf('%s=connect(%s);',char(channel_name),char(channel_name)));
    end
end
if get_output_channel_info('size')>0
    for i=1:get_output_channel_info('size')
        channel_name=get_output_channel_info('channel_name',i);
        eval(sprintf('global %s;',channel_name));
        eval(sprintf('%s=connect(%s);',char(channel_name),char(channel_name)));
    end
end