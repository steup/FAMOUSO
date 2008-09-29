function counter=get_events()
    counter=0;
    if get_input_channel_info('size')>0
        for i=1:get_input_channel_info('size')
            channel_name=get_input_channel_info('channel_name',i);
            eval(sprintf('global %s;',channel_name));
            eval(sprintf('[aux %s]=get_event(%s);',channel_name, channel_name));
            if aux>=1
                counter=counter+1;
            end
        end
    end