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

function output = get(a,argument)
%GET Summary of this function goes here
%   Detailed explanation goes here
    output=[];
    switch argument
        case 'xdata'
            output=a.xdata;
        case 'ydata'
            output=a.ydata;
        case 'patchXData' 
            if strcmp(class(a.patch),'double')
                output = get(a.patch,'XData');	% Define the patch
            else
                output=a.patch.x;
            end           
        case 'patchYData'
            if strcmp(class(a.patch),'double')
                output = get(a.patch,'YData');	% Define the patch
            else
                output=a.patch.y;
            end   
        case 'trigger_delay'
            output=a.trigger.delay;
        case 'trigger_period'
            output=a.trigger.period;
         case 'trigger_mode'
            output=a.trigger.mode; 
        case 'crashed'
            output=a.crashed;
        case 'position'
            output=a.position;
        case 'heading'
            output=heading;
        case 'velocity'
            output=velocity;
         case 'name'
            output=velocity;
         case 'number'
            output=number;            
        case {'af','color','scale','patch','power','accel'}
            eval(sprintf('output=a.%s;',argument));
        otherwise
            error('Wrong argument for class simrobot !!!');
    end
end
