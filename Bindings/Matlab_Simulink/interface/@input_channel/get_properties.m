function val = get_properties(a, propName)
% GET Get asset properties from the specified object
% and return the value
    switch propName
    case 'subject'
       val = a.subject;
    case 'timeout'
       val = a.timeout;
    case 'number'
       val = a.number;
    case 'comment'
       val = a.comment;
    case 'interaction_type'
       val = a.interaction_type;
    case 'connection'
       val = a.connection;
    case 'function_name'
       val = a.function_name;
    otherwise
       error([propName,' Is not a valid asset property'])
end