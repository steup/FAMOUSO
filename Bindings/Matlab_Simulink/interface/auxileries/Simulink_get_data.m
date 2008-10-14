function value=Simulink_get_data(subject, method)
	channel_name=id2name('input',subject);
%     dbstop Simulink_get_data at 4
    eval(sprintf('global %s;',channel_name));
    eval(sprintf('[data %s] =get_data(%s, method);',channel_name,channel_name));
    
    switch method
        case 'newest'
%             dbstop Simulink_get_data.m at 10
            value=0;
            if ~isempty(data)
                if data(1,1)~=0
                    value=1;
                end
%                 aux = max(size(data));
%                 if aux>8 
%                     aux=8;
%                 end
%                 for i=1:aux
%                     value(i)=data(i);
%                 end
            end
%            value=double(value);

        case 'all'
            % hier wird jetzt fuer das Szenario geschummelt und der
            % erstbeste Datansatz mit der richtigen Robot ID gesucht
                if strcmp(channel_name,'distance')
                    if ~isempty(data)
                        data=double(data);
                        a=find(data(:,3)==4);
            %             value=[[0 0 0];[0 0 0]];
                        value=[0 0 0];
                        if ~isempty(a)
                            value(1,1)=data(a(1),1);
                        end
                        a=find(data(:,3)==3);
                        if ~isempty(a)
                            value(1,3)=data(a(1),1);
                            if value(1,3)==255
                                value(1,3)=0;
                            end
                        end
                    else
                        value=[0 0 0];
                    end
                end
                if strcmp(channel_name,'HumanDet')
                    value=0;
                     if ~isempty(data)
                        data=double(data);
                        if sum(data(:,1))>0
                            value=1;
                            disp('jetzt')
                        end
                     end
                end
    end