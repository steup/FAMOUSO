function val = get_properties(a, propName)
% GET Get asset properties from the specified object
% and return the value
    switch propName
    case 'host'
       val = a.host;
    case 'port'
       val = a.port;
    case 'tcpobj'
       val = a.tcpobj;
    case 'tcp_active'
       val=0;
       if strcmp(a.interaction_type,'callback')
           if strcmp(a.tcpobj.status,'open')
               val=1;
           end
       else
           if (pnet(a.tcpobj,'status')==11)
               val=1; 
           end
       end
    case 'interaction_type'
       val = a.interaction_type;
    case 'message_length'
       val = a.message_length;
    case 'channel_name'
       val = a.channel_name;
    otherwise
       error([propName,' Is not a valid asset property'])
end