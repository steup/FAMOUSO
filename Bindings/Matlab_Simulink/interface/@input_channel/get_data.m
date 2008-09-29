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
    case 'sum'
       disp('have to be implemented !!!!')
    otherwise
       error([propName,' Is not a valid asset property'])
    end
end