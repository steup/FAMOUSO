function [val,a] = get_data(a, propName)
% GET Get asset properties from the specified object
% and return the value
    switch propName
     case 'newest'
       if a.index~=0
           if a.messages(a.index).flag==1
               val = a.messages(a.index).values;
           else
               val=[];
           end
       else
           val=[]; 
       end
    case 'all'
       val=[];
       if a.index~=0
           % search for all new event data
           num=find([a.messages.flag]==1);
           if ~isempty(num)
               for i=1:length(num)
                   if ~isempty(a.messages(num(i)).values)
                       val= [val; a.messages(num(i)).timestamp a.messages(num(i)).values(1:a.messages(num(i)).length)'];
                   end
                   a.messages(num(i)).flag=0;
               end
           end
       end


    case 'sum'
       disp('have to be implemented !!!!')
    otherwise
       error([propName,' Is not a valid asset property'])
    end
end