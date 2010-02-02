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

function obj = simrobot(varargin)
if ~isempty(varargin)
    aux=max(size(varargin));
else
    aux=0;
end
switch aux
    case 0
        error('Simrobot needs parameter for instances !!!')
    case 12
        obj.name = varargin{1};
        obj.number =  varargin{2}; 
        % In degrees, 0 = facing east [°]
        obj.heading = varargin{3};
        % Control file
        obj.af = varargin{4};
        obj.color = varargin{5};
        obj.scale = varargin{6};  
        % Contour data
        obj.xdata = varargin{7}';
        obj.ydata = varargin{8}';
        
        obj.sensors = varargin{9};
        obj.position = varargin{10};
        % Absolute position
        obj.trigger = varargin{11};
        
        aux=varargin{12};
        obj.R=aux(1);
        obj.la=aux(2); 
        obj.lb=aux(3);
        
        obj.power = false;
        obj.crashed = false;
        
        obj.patch=[];
        obj.line=[];

        % Speed of left and right wheel
        obj.velocity = [0 0];
        % Acceleration of left and right wheel
        obj.accel = [0 0];
        
        obj.patchcircle=max(sqrt(obj.xdata.^2+obj.ydata.^2));
        %todo Berechnung einfuegen !!!
        obj.sensorcircle=50;
        obj.panelControled=false;
    otherwise
        disp('Wrong number of input arguments for simrobot contructor')
end