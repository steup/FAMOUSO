function aux=get_input_channel_info(varargin)
global input_channel_IDs;
    switch varargin{1}
        case 'channel_name'
            aux=deblank(input_channel_IDs(varargin{2}).name);
        case 'subject_hex'
            aux=deblank(input_channel_IDs(varargin{2}).subject);
        case 'subject_dec'
            aux=0;
            value=deblank(input_channel_IDs(varargin{2}).subject);
            for i=1:8
                aux=aux+hex2dec(value(2*i-1:2*i))*256^(8-i); 
            end
        case 'size'
            aux=max(size(input_channel_IDs));
        otherwise
            disp('something wrong')
            aux=[];
    end
end