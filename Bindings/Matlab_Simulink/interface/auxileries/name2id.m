function subject=name2id(direction,channel_name)
subject=[];
switch direction
    case 'input'
        for i=1:get_input_channel_info('size')
            if strcmp(get_input_channel_info('channel_name',i),channel_name)
                subject=get_input_channel_info('subject_dec',i);
                break
            end
        end
     case 'output'
        for i=1:get_output_channel_info('size')
            if strcmp(get_output_channel_info('channel_name',i),channel_name)
                subject=get_output_channel_info('subject_dec',i);
                break
            end
        end
end
         
    