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

function in=myinpolygon(x,y,xs,ys)
% [xs,ys] ... Stützstellen des Polygons
% [x, y]  ... Punkte des 

% Mit
%     | x(j)  |   | y(j)  |
% A = |xs(i)  | x |ys(i)  |
%     |xs(i+1)|   |ys(i+1)|
%
% ergibt sich
% A = xs(i)*ys(i+1)- ys(i)*xs(i+1) +
%     xs(i+1)*y(j) - x(j)*ys(i+1) +
%     x(j)*ys(i)   - y(j)*xs(i)
% zusammengefasst auf konstante Anteile
% A = m + x(j)*n + y(j)*o
% mit
% m(i) = xs(i)*ys(i+1)-ys(i)*xs(i+1) und
% n(i) = ys(i)-ys(i+1)
% o(i) = xs(i+1)-xs(i)
% Die konstanten Anteile werden im voraus berechnet.

in=zeros(length(x),1);

m=zeros(1,length(xs)-1);
n=zeros(1,length(xs)-1);
o=zeros(1,length(xs)-1);

for i=1:length(xs)-1
    m(i) = xs(i)*ys(i+1)-ys(i)*xs(i+1);
    n(i) = ys(i)-ys(i+1);
    o(i) = xs(i+1)-xs(i);
end

aux=[ones(length(x),1) x y]*[m; n; o];
in=~(max(aux')>0 & min(aux')<0);