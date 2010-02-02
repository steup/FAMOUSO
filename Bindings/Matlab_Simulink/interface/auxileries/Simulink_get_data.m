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

function value=Simulink_get_data(subject, method)
	channel_name=id2name('input',subject);
%     dbstop Simulink_get_data at 4
    eval(sprintf('global %s;',channel_name));
%     dbstop in Simulink_get_data at 6
    eval(sprintf('[data %s] =get_data(%s, method);',channel_name,channel_name));
    
    switch method
        case 'newest'
%             dbstop Simulink_get_data.m at 10
            value=0;
            if ~isempty(data)
                if data(1,1)~=0
                    value=1;
                end
%                 aux = max(size(data));
%                 if aux>8 
%                     aux=8;
%                 end
%                 for i=1:aux
%                     value(i)=data(i);
%                 end
            end
%            value=double(value);

        case 'all'
            % hier wird jetzt fuer das Szenario geschummelt und der
            % erstbeste Datansatz mit der richtigen Robot ID gesucht
%                 dbstop in Simulink_get_data at 30
                if strcmp(channel_name,'distance')
                    if ~isempty(data)
                        data=double(data);
                        a=find(data(:,3)==4);
            %             value=[[0 0 0];[0 0 0]];
                        value=[0 0 0];
                        if ~isempty(a)
                            value(1,1)=data(a(1),1);
                        end
                        a=find(data(:,3)==3);
                        if ~isempty(a)
                            value(1,3)=data(a(1),1);
                            if value(1,3)==255
                                value(1,3)=0;
                            end
                        end
                    else
                        value=[0 0 0];
                    end
                end
                if strcmp(channel_name,'HumanDet')
                    value=0;
                     if ~isempty(data)
                        data=double(data);
                        if sum(data(:,1))>0
                            value=1;
                            disp('jetzt')
                        end
                     end
                end
    end