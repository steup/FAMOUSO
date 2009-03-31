function a = set_properties(a,varargin)
% SET Set asset properties and return the updated object
propertyArgIn = varargin;
while length(propertyArgIn) >= 2,
   prop = propertyArgIn{1};
   val = propertyArgIn{2};
   propertyArgIn = propertyArgIn(3:end);
   switch prop
   case 'subject'
      a.Subject = val;
   case 'timeout'
      a.timeout = val;
   case 'number'
      a.number = val;
   case 'connection'
      a.connection=val;      
   otherwise
      error('input channel properties: subject timeout number tcpobj')
   end
end