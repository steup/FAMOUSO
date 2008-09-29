function val = display(a)
% GET Get asset properties from the specified object
% and return the value
     output=sprintf('Host:             %s\nPort:             %i\nPackage:          %s\nTCPIPstate:       %d\n',...
     char(a.host), a.port, a.interaction_type, a.tcp_active);
    disp(output)
end