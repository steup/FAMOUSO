function aux=get_output_channel_info(varargin)
global output_channel_IDs;
    switch varargin{1}
        case 'channel_name'
            aux=deblank(output_channel_IDs(varargin{2}).name);
        case 'subject_hex'
            aux=deblank(output_channel_IDs(varargin{2}).subject);
        case 'subject_dec'
            aux=0;
            value=deblank(output_channel_IDs(varargin{2}).subject);
            for i=1:8
                aux=aux+hex2dec(value(2*i-1:2*i))*256^(8-i); 
            end
        case 'size'
            aux=max(size(output_channel_IDs));
        otherwise
            disp('something wrong')
            aux=[];
    end
