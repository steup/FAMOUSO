function a = set_properties(a,varargin)
% SET Set asset properties and return the updated object
propertyArgIn = varargin;
while length(propertyArgIn) >= 2,
   prop = propertyArgIn{1};
   val = propertyArgIn{2};
   propertyArgIn = propertyArgIn(3:end);
   switch prop
    case 'tcp_active'
      a.tcp_active = val;
    case 'interaction_type'
      a.interaction_type = val;
    case 'tcpobj'
      a.tcpobj=val;
    case 'host'
      a.host=val;
    case 'port'
      a.port=val;
    case 'channel_name'
      a.channel_name=val;
     case 'message_length'
      a.message_length=val;
   otherwise
      error('TCPIP_connection wrong properties')
   end
end