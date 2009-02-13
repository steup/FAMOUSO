%% Channel definition
% Definition of the FAMOUSO channels
function [input, output, maxLenght]=FAMOUSOconfig()

%% Definition of "interesting" FAMOUSO channels
%       subject                 type       callback_function number  comment          '
input =[['56656C6F63697479      callback    channel2robot.m     20      Velocity        '],
        ['506F736974696F6E      callback    positionCallback.m  5       Position        ']];

output=['44697374616E6365                                               Distance'];

%% maximum length of the data in Byte
maxLenght=8;