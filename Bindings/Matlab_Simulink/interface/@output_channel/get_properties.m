function val = get_properties(a, propName)
% GET Get asset properties from the specified object
% and return the value
    switch propName
    case 'subject'
       val = a.subject;
    case 'comment'
       val = a.comment;
    otherwise
       error([propName,' Is not a valid asset property'])
end