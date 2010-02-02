%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%
%% Copyright (c) 2008-2010 Sebastian Zug <zug@ivs.cs.uni-magdeburg.de>
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

function a = TCPIP_connection(varargin)
    switch nargin
        case 0
        % if no input arguments, create a default object
            a.tcpobj = [];
            a.interaction_type = [];
            a.tcp_active=0;
            a.host=[];
            a.port=[];
            a.message_length=[];
            a.channel_name=[];
            a = class(a,'TCPIP_connection');
        case 1
        % if single argument of class input_Channel, return it
        % Copy constructor
           if (isa(varargin{1},'TCPIP'))
             	a = varargin{1};
           else
                error('Wrong parameter for TCPIP constructor')
           end 
        case 3
            a.tcpobj = [];
            a.interaction_type = [];
            a.tcp_active=0;
            a.channel_name=[];
            a.host=varargin{1};
            a.port=varargin{2};
            a.message_length=varargin{3};
            a = class(a,'TCPIP_connection');
        otherwise
           error('Wrong number of input arguments')
    end
end


%package='pnet';