function a = activate( a )
%ACTIVATE Summary of this function goes here
%   Detailed explanation goes here

        sina = sin(-a.heading*pi/180);
        cosa = cos(-a.heading*pi/180);
        transM = [[cosa -sina];[sina cosa]];
        
        x=[a.xdata a.ydata];
        conture = a.scale*x*transM;
  
        %% Kann man beim Plotbefehl die Ausgabe unterdrücken, sprich ich ""
        

        a.patch = patch(	'XData',a.position(1)+conture(:,1), ...
                            'YData',a.position(2)+conture(:,2), ...
                            'FaceColor',a.color, ...
                            'EdgeColor','k', ...
                            'tag',a.name, ...
                            'EraseMode','xor');	% Define the patch

        a.line = line(	    'XData',[1 2 3], ...
                            'YData',[3 4 5], ...
                            'tag','Tralla', ...
                            'EraseMode','xor');	% Define the sensor observation area

end
