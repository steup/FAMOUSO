function value=char2int(data)
    for i=1:max(size(data))
        if data(i)>127
            value(i)=data(i)-256;
        else
            value(i)=data(i); 
        end
    end