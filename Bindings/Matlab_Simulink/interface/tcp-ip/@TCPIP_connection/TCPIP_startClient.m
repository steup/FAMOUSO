%% StartTCP Client depending on the channel definition list. 
% If a callback function ist included the DAQ Toolbox is choosen. Otherwise
% the pnet library is used.
function a=TCPIP_startClient(a)

% Starting connection
if strcmp(get_properties(a,'interaction_type'),'callback')
    % creation of a tcpip object
    tcpobj = tcpip(get_properties(a,'host'),...
             get_properties(a,'port'));

    % Activation of the Callback function
    tcpobj.BytesAvailableFcnCount = get_properties(a,'message_length');
    tcpobj.BytesAvailableFcnMode = 'byte';
    tcpobj.BytesAvailableFcn = {@Callback_TCP_BytesAvailableFcn,a};

    try
        fopen(tcpobj)
    catch
        error('DAQ TCP/IP connection is not available !!!');
    end    
else
    tcpobj=pnet('tcpconnect',get_properties(a,'host'),... 
                             get_properties(a,'port'));
      
    if ~(pnet(tcpobj,'status')==11)
        error('pnet connection is not available !!!');
    end
end

 a=set_properties(a,'tcpobj',tcpobj);
 a=set_properties(a,'tcp_active',1);