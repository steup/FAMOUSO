function in=myinpolygon(x,y,xs,ys)
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