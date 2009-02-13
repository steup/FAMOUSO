function value=char2int(data)
%     for i=1:max(size(data))
%         if data(i)>127
%             value(i)=double(data(i))-256;
%         else
%             value(i)=double(data(i)); 
%         end
%     end
    value=double(data);
    value(value>127)=value(value>127)-256;