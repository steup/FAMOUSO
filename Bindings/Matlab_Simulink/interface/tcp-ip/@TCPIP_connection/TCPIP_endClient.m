%% StartTCP Client depending on the channel definition list. 
% If a callback function ist included the DAQ Toolbox is choosen. Otherwise
% the pnet library is used.
function a=TCPIP_endClient(a)

% Starting connection
if strcmp(get_properties(a,'interaction_type'),'callback')
    if get_properties(a,'tcp_active');
        fclose(get_properties(a,'tcpobj'));
    end
else
    pnet(get_properties(a,'tcpobj'),'close');
end

 a=set_properties(a,'tcp_active',0);