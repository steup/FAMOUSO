function channel_name=id2name(direction, subject)
switch direction
    case 'input'
        for i=1:get_input_channel_info('size')
            if subject==get_input_channel_info('subject_dec',i);
                channel_name=get_input_channel_info('channel_name',i);
                break
            end
        end
    case 'output'
        for i=1:get_output_channel_info('size')
            if subject==get_output_channel_info('subject_dec',i);
                channel_name=get_output_channel_info('channel_name',i);
                break
            end
        end
end