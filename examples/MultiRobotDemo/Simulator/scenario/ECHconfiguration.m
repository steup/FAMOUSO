%% Channel definition
% Definition of the FAMOUSO channels
function [input, output, maxLenght]=FAMOUSOconfig()

%% Definition of "interesting" FAMOUSO channels

% The input channel definition must include the same number of characters
% !!! Hence, pay attention to this in the following lines.
%       subject                 type       callback_function    number  comment          '
input =[['56656C6F63697479      callback    VelocityCallback.m  20      Velocity'],
        ['48756D616E446574      callback    HumanDetCallback.m  5       HumanDet']
        ['506F736974696F6E      callback    PositionCallback.m  5       Position']
        ['5374656572696E67      callback    SteeringCallback.m  5       Steering']];

output=[['44697374616E6365                                              Distance'],
        ['437261736865645F                                              Crashed_']];

%% maximum length of the data in Byte
maxLenght=8;