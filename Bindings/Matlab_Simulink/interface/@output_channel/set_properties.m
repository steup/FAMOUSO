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
   case 'comment'
      a.comment = val;
    case 'connection'
      a.connection = val;
   otherwise
      error('Asset properties: Descriptor, Date, CurrentValue')
   end
end