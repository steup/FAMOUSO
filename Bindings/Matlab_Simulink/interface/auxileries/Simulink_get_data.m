function value=Simulink_get_data(subject, method)
	channel_name=id2name('input',subject);
    eval(sprintf('global %s;',channel_name));
    eval(sprintf('[data %s] =get_data(%s, method);',channel_name,channel_name));
    value=[0 0 0 0 0 0 0 0];
    if ~isempty(data)
        aux = max(size(data));
        if aux>8 
            aux=8;
        end
        for i=1:aux
            value(i)=data(i);
        end
    end
    value=double(value);