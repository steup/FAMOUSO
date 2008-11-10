function a = activate( a, mode )
%ACTIVATE Summary of this function goes here
%   Detailed explanation goes here

        sina = sin(-a.heading*pi/180);
        cosa = cos(-a.heading*pi/180);
        transM = [[cosa -sina];[sina cosa]];
        
        x=[a.xdata a.ydata];
        conture = a.scale*x*transM;
  
        if strfind(mode, 'vis')
            a.patch = patch('XData',a.position(1)+conture(:,1), ...
                'YData',a.position(2)+conture(:,2), ...
                'FaceColor',a.color, ...
                'EdgeColor','k', ...
                'tag',a.name, ...
                'EraseMode','xor');	% Define the patch

            a.line = line('XData',[1 2 3], ...
                'YData',[3 4 5], ...
                'tag','Tralla', ...
                'EraseMode','xor');	% Define the sensor observation area
        else
            a.patch.x=a.position(1)+conture(:,1);
            a.patch.y=a.position(2)+conture(:,2);
            a.line.x=[];
            a.line.y=[];
        end
end