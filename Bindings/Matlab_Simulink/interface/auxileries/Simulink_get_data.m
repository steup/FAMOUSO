function value=Simulink_get_data(subject, method)
	channel_name=id2name('input',subject);
    eval(sprintf('global %s;',channel_name));
    eval(sprintf('[data %s] =get_data(%s, method);',channel_name,channel_name));
    
    
    switch method
        case 'newest'
            if ~isempty(data)
                value=[0 0 0 0 0 0 0 0];
                aux = max(size(data));
                if aux>8 
                    aux=8;
                end
                for i=1:aux
                    value(i)=data(i);
                end
            end
           value=double(value);
        case 'all'
            % hier wird jetzt fuer das Szenario geschummelt und der
            % erstbeste Datansatz mit der richtigen Robot ID gesucht
            data=double(data);
            a=find(data(:,1)==1);
            value=[0 0 0];
            if ~isempty(a)
                value=data(a(1),:);
            end
    end