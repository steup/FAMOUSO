%% subscribtion

function FAMOUSOannounceAll()
if get_output_channel_info('size')>0
    for i=1:get_output_channel_info('size')
        channel_name=get_output_channel_info('channel_name',i);
        eval(sprintf('global %s;',channel_name));
        eval(sprintf('%s=announce(%s);',char(channel_name),char(channel_name)));
    end
end