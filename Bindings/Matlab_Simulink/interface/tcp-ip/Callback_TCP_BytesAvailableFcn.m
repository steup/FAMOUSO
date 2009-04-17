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

function Callback_TCP_BytesAvailableFcn(obj,event,tcpobj)

%% obtain the correct global object based on the channel name inside tcpobj
    if get_input_channel_info('size')>0
        for i=1:get_input_channel_info('size')
            channel_name=get_input_channel_info('channel_name',i);
            if strcmp(channel_name, get_properties(tcpobj,'channel_name'))
                % get global channel object
                eval(sprintf('global %s;',channel_name));
                break;
            end
        end
    end

%% read all messages
    persistent data;
    % add the residue of the last cycle to the new values
    input=TCPIP_read(eval(sprintf('get_properties(%s,''connection'');',channel_name)));
    if isempty(input)
        return
    end
%     size(input)
    if isempty(data)
        data=input;
    else
        data=[data; input];
    end
%     size(data)

    subject_index=strfind(char(data)',channel_name);
    if ~isempty(subject_index)
        for i=1:length(subject_index)
            current_length=data(subject_index(i)+8)*256^3+data(subject_index(i)+9)*256^2+data(subject_index(i)+10)*256+data(subject_index(i)+11);
            %         data(subject_index(i)+8:subject_index(i)+11)
            eval(sprintf('%s=inc_index(%s);',channel_name,channel_name));
            t=clock;
            if i<length(subject_index)
                if subject_index(i)+12+current_length<subject_index(i+1)
                    values=char(data(subject_index(i)+12:subject_index(i)+11+current_length));
                    eval(sprintf('%s=set_data(%s,t(6),current_length,values);',channel_name,channel_name));
                end
            else
                if length(data)>=subject_index(i)+11+current_length
                    values=char(data(subject_index(i)+12:subject_index(i)+11+current_length));
                    eval(sprintf('%s=set_data(%s,t(6),current_length,values);',channel_name,channel_name));
                else
                    data=data(subject_index(i):length(data));
                end
            end
        end
    else
        % no complete message data
    end
    
    
    
    
    
    
%     number=length(data);
%     k=1;
% 
%     
%     
%     %% Searching for valid data
%     while(true)
%         % contains the current data complete header?
% %          fprintf('#')
%         if (number-k)>12
% %             current_subject=[];
% %             for i=1:8
% %                 aux=dec2hex(double(data(k+i)));
% %                 if max(size(aux))<2
% %                     current_subject=[current_subject '0' aux];
% %                 else
% %                     current_subject=[current_subject aux];
% %                 end
% %             end
%             current_length=data(k+9)*256^3+data(k+10)*256^2+data(k+11)*256+data(k+12);
%             current_length=bitshift(data(k+9),24)+bitshift(data(k+10),16)+bitshift(data(k+11),8)+data(k+12);
%             % is the data message complete?
%             if ((number-(k+12))>=current_length)
%                 eval(sprintf('%s=inc_index(%s);',channel_name,channel_name));
%                 % a=inc_index(a);
%                 % Copy the data into the global variable
%                 t=clock;
%                 values=char(data(k+12+1:k+12+current_length));
% %                 double(values)
%                 eval(sprintf('%s=set_data(%s,t(6),current_length,values);',channel_name,channel_name));
% 
%                 % k points on the first value of the next message
%                 k=k+12+current_length+1;
%                 % no valid data any more
%                 if number<k
%                     data=data(k:size(data,2));
%                     break;
%                 end
%             else
%                 % no complete message data
%                 break;
%             end
%         else
%             % no complete message header
%             break;
%         end
%         % Reading the rest
%     end
% %      fprintf('\n')
    %% Call of the corresponding function
    function_name=eval(sprintf('get_properties(%s,''function_name'');',channel_name));
    if ~strcmp(function_name,'-')
        [pathstr, name, ext, versn] = fileparts(function_name);
        try
            eval(sprintf('%s(%s)',name,channel_name));
        catch
            error(['Wrong callback function for FAMOUSO event' function_name])
        end
    end
end
