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

%% FAMOUSO Channel Settings

% and definition of the channel objects
function FAMOUSOinit(TCPIPsettings)
disp('Starting ECH ...');
%% Defintion of the classes
global input_channel_IDs;
global output_channel_IDs;
input_channel_IDs=struct('name',{},'subject',{});
output_channel_IDs=struct('name',{},'subject',{});

%% Reading the definition file
fprintf('Reading FAMOUSO channels for subscribe / publish ...\n');
[input_def, output_def, maxLength]=ECHconfiguration();
fprintf('\n           subject            comment    type\n------------------------------------------------------\n');

%% input
% initionlisation of the channel and events objects
for i=1:size(input_def,1)
    try
        [subject i_type function_name number comment]=strread(input_def(i,:), '%s %s %s %d %s');
        error=0;
    catch
        disp('Wrong FAMOUSO input channel definition, please check this');
        error=1;
    end
    if error==0
        % TODO: I missed a consitency check of the channel definition here !!!
        % something like this
        % -----------------------------------------------------------------
        if strcmp(i_type,'polling') | strcmp(i_type,'callback')
            if strcmp(i_type,'polling')
                function_name='-';
            end
        else
            error('Unknown TCP\IP configuration - ''polling'' or ''callback'' allowed only');
        end
        % -----------------------------------------------------------------       
        fprintf('subscribe  %s   %s   %s \n', char(subject), char(comment),char(i_type));
        eval(sprintf('global %s;',char(comment)));
        eval(sprintf('%s=input_channel(char(subject), number, char(comment), char(function_name));',char(comment)));
        input_channel_IDs(i).name=char(comment);
        input_channel_IDs(i).subject=char(subject);
    end
end

% Initialisation of each TCP/IP channel
if get_input_channel_info('size')>0
    aux=TCPIP_connection();
    aux=set_properties(aux,'host',TCPIPsettings.host);
    aux=set_properties(aux,'port',TCPIPsettings.port);
    aux=set_properties(aux,'message_length',TCPIPsettings.message_length); 
    for i=1:get_input_channel_info('size')
        channel_name=get_input_channel_info('channel_name',i);
        aux=set_properties(aux,'channel_name',channel_name);
        eval(sprintf('global %s;',channel_name));
        aux=set_properties(aux,'interaction_type',char(i_type));     
        eval(sprintf('%s=set_properties(%s,''connection'',aux);',char(channel_name),char(channel_name)));
    end
end


%% output
% initionlisation of the channel and events objects
for i=1:size(output_def,1)
    try
        [subject comment]=strread(output_def(i,:), '%s %s');
    catch
        disp('Wrong FAMOUSO output channel definition, please check this');
    end
    if error==0
        fprintf('publish    %s   %s\n', char(subject), char(comment));
        eval(sprintf('global %s;',char(comment)));
        eval(sprintf('%s=output_channel(char(subject), char(comment));',char(comment)));
        output_channel_IDs(i).name=char(comment);
        output_channel_IDs(i).subject=char(subject);
    end
end
fprintf('\n');

% Initialisation of each TCP/IP channel
if get_output_channel_info('size')>0
    aux=TCPIP_connection();
    aux=set_properties(aux,'host',TCPIPsettings.host);
    aux=set_properties(aux,'port',TCPIPsettings.port);
    aux=set_properties(aux,'message_length',TCPIPsettings.message_length);
    for i=1:get_output_channel_info('size')
        channel_name=get_output_channel_info('channel_name',i);
        eval(sprintf('global %s;',channel_name));
        aux=set_properties(aux,'interaction_type',char(i_type));
        eval(sprintf('%s=set_properties(%s,''connection'',aux);',char(channel_name),char(channel_name)));
    end
end