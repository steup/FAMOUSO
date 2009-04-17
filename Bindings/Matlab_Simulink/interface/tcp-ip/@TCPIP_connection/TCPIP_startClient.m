%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%
%% Copyright (c) 2008, 2009 Sebastian Zug <zug@ivs.cs.uni-magdeburg.de>
%% All rights reserved.
%%
%%    Redistribution and use in source and binary forms, with or without
%%    modification, are permitted provided that the following conditions
%%    are met:
%%
%%    * Redistributions of source code must retain the above copyright
%%      notice, this list of conditions and the following disclaimer.
%%
%%    * Redistributions in binary form must reproduce the above copyright
%%      notice, this list of conditions and the following disclaimer in
%%      the documentation and/or other materials provided with the
%%      distribution.
%%
%%    * Neither the name of the copyright holders nor the names of
%%      contributors may be used to endorse or promote products derived
%%      from this software without specific prior written permission.
%%
%%
%%    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
%%    IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
%%    TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
%%    PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
%%    OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
%%    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
%%    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
%%    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
%%    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
%%    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
%%    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
%%
%%
%% $Id$
%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%% StartTCP Client depending on the channel definition list. 
% If a callback function ist included the DAQ Toolbox is choosen. Otherwise
% the pnet library is used.
function a=TCPIP_startClient(a,callback)

% Starting connection
if strcmp(get_properties(a,'interaction_type'),'callback')
    % creation of a tcpip object
    tcpobj = tcpip(get_properties(a,'host'),...
             get_properties(a,'port'));

    if callback==1
        % Activation of the Callback function
        tcpobj.BytesAvailableFcnCount = get_properties(a,'message_length');
        tcpobj.BytesAvailableFcnMode = 'byte';
        tcpobj.BytesAvailableFcn = {@Callback_TCP_BytesAvailableFcn,a};
    end

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