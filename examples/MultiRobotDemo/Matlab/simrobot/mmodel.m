function [xspeed, yspeed, rotspd] = mmodel(omega1, omega2, heading)

% Model constants, keep consistent with those ones in invmodel.m!
R = 1;
la = 3;
lb = 0;

v=(R/(2*la))*[[-lb*omega1+lb*omega2];[-la*omega1-la*omega2]];

heading = pi*heading/180;
cosa=cos(heading + pi);
sina=sin(heading + pi);

mat_aux=[[cosa sina];[cosa sina]];
speed= v'*mat_aux;

xspeed=speed(1);
yspeed=speed(2);

rotspd = ((R/(2*la))*(-omega1 + omega2)*180/pi);